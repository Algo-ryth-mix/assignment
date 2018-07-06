#pragma once

#include "Tiles.h"
#include "LevelLoader.h"
#include "Item.h"
#include "EnemyController.h"
#include <filesystem>
#include <algorithm>
#include "helper.h"

const int AVAILABLE_TIME_DEFAULT = 15; //seconds

class Player
{
public:

	//create a Player
	Player(gameboard* board) : m_board(board)
	{
		//init gui
		gui.fnt.loadFromFile("VT323-Regular.ttf");

		gui.healthText.setFont(gui.fnt);
		gui.healthText.setString("Health:");
		gui.healthText.setCharacterSize(20);
		gui.healthText.setFillColor(sf::Color::White);
		gui.healthText.setPosition(850,40);

		gui.healthBar.setSize({static_cast<float>(m_health),10});
		gui.healthBar.setPosition({850,70});
		gui.healthBar.setFillColor(sf::Color::Green);

		gui.healthEmptyBar.setSize({static_cast<float>(m_health),10});
		gui.healthEmptyBar.setPosition({850,70});
		gui.healthEmptyBar.setFillColor(sf::Color(120,120,120));

		gui.pointsText.setFont(gui.fnt);
		gui.pointsText.setString("Points:");
		gui.pointsText.setCharacterSize(20);
		gui.pointsText.setFillColor(sf::Color::White);
		gui.pointsText.setPosition(850,80);

		gui.pointsAmountText.setFont(gui.fnt);
		gui.pointsAmountText.setString(std::to_string(m_points).c_str());
		gui.pointsAmountText.setCharacterSize(20);
		gui.pointsAmountText.setFillColor(sf::Color::White);
		gui.pointsAmountText.setPosition(850,100);

		gui.elapsedTime.setFont(gui.fnt);
		gui.elapsedTime.setString("0");
		gui.elapsedTime.setCharacterSize(30);
		gui.elapsedTime.setFillColor(sf::Color::Red);
		gui.elapsedTime.setPosition(850,10);

		gui.inventoryText.setFont(gui.fnt);
		gui.inventoryText.setString("Inventory:");
		gui.inventoryText.setCharacterSize(20);
		gui.inventoryText.setFillColor(sf::Color::White);
		gui.inventoryText.setPosition(850,120);

		gui.inventoryContentsText.setFont(gui.fnt);
		gui.inventoryContentsText.setString("Empty");
		gui.inventoryContentsText.setCharacterSize(20);
		gui.inventoryContentsText.setFillColor(sf::Color::White);
		gui.inventoryContentsText.setPosition(850,140);

		//init player
		spawn();
	}

	//spawn (or respawn) the player
	void spawn() //respawn at spawn tile
	{
		for_all_tiles(m_board,[&](int x,int y,Tile & t)
		{
			if(t.state == IS_WORLD_SPAWN)
			{
				if(m_board->at(m_x).at(m_y).state == IS_PLAYER) //check if we got overwritten
				{
					m_board->at(m_x).at(m_y).state = IS_GROUND;
				}
				t.state = IS_PLAYER;
				m_x = x;
				m_y = y;
			}
		});
		m_timer.restart();
	}

	//get the players posiiton on the board
	sf::Vector2i getPosition() const
	{
		return {m_x,m_y};
	}

	//move Player && do all the Player Logic when moved
	void setPosition(int x,int y)
	{
		//stop at world edge
		if(x >= 32 || y >= 32 || x < 0 ||y < 0) return;

		//stop at wall
		if(m_board->at(x).at(y).state == IS_WALL)return;

		//handle portals
		if(m_board->at(x).at(y).state == IS_PORTAL)
		{
			//load new level
			LevelLoader::loadLevel(m_board,m_board->at(x).at(y).extraData);
			EnemyController::get().loadEnemies(m_board->at(x).at(y).extraData);

			//respawn in level
			spawn();
			return;
		}

		//handle enemies
		if(m_board->at(x).at(y).state == IS_ENEMY)
		{
			Enemy& e = EnemyController::get().getEnemyAt(x,y);
			
			//do damage calculations

			int total_damage = BASE_ATK;
			float total_defense = BASE_DEF;

			for(size_t i = 0; i < m_inventory.size();i++)
			{
				Item& it = m_inventory[i];
				total_damage += it.atk_buf;
				total_defense += it.def_buf;
			
				//check if item is used up
				it.num_uses--;
				if(it.num_uses == 0)
				{
					//if it is remove it
					m_inventory.erase(m_inventory.begin()+i);
				}
			}

			if(e.extraData.find("require") != e.extraData.end())
			{
				for(const auto& item : m_inventory)
				{
					if(e.extraData["require"] == item.id)
					{
						//apply damage to enemy (only if the player has the required weapon)
						e.health -= total_damage;
					}
				}
			}
			else
			{
				//apply damage to enemy
				e.health -= total_damage;
			}


			if(e.health > 0)
			{
				//apply damage to player
				m_health -= (e.atk / total_defense);
				return;
				
			}

			//add enemy drops to player
			for(const auto& drop : e.drops)
			{
				addToInventory(drop);
			}

			//add points to player
			m_points+=e.value;

			if(e.extraData.find("destroys") != e.extraData.end())
			{
				using array_t =  nlohmann::json::array_t;
				for(auto& elem : e.extraData["destroys"].get<array_t>())
				{
					EnemyController::get().removeEnemyAt(elem.get<array_t>()[0].get<int>(),
														 elem.get<array_t>()[1].get<int>());

				}
			}

			//remove enemy
			EnemyController::get().removeEnemyAt(x,y);

		}

		//"swap" places with the tile
		m_board->at(m_x)	.at(m_y)	.state = IS_GROUND;
		m_board->at(x)		.at(y)		.state = IS_PLAYER;
		m_x = x;
		m_y = y;
	}

	//show all the GUI elements
	void showGUI(sf::RenderWindow& wnd)
	{
		gui.healthBar.setSize({static_cast<float>(m_health),10});


		wnd.draw(gui.healthEmptyBar);
		wnd.draw(gui.healthBar); //draw over the "empty" bar

		wnd.draw(gui.healthText);

		gui.pointsAmountText.setString(std::to_string(m_points).c_str());
		wnd.draw(gui.pointsAmountText);
		wnd.draw(gui.pointsText);

		//construct text for inventory
		{
			std::string inventoryText;
			for(const auto& item : m_inventory) inventoryText += "- " + item.name +"\n";
			gui.inventoryContentsText.setString(inventoryText.c_str());
		}
		wnd.draw(gui.inventoryContentsText);
		
		wnd.draw(gui.elapsedTime);
		wnd.draw(gui.inventoryText);

	}

	//pretty self explanatory
	bool is_dead() const
	{
		return m_health <= 0;
	}

	//update some internal variables every tick
	void update()
	{
		//did the player take longer than 10 seconds to complete the level ?
		if(m_timer.getElapsedTime().asMilliseconds() > AVAILABLE_TIME_DEFAULT * 1000)
		{
			m_health = 0; //yes level failed
		}

		gui.elapsedTime.setString(std::to_string(m_timer.getElapsedTime().asSeconds()).c_str());

		namespace fs = std::experimental::filesystem;

		//check if inventory got modified
		if(m_invDirty){
			
			//if yes search for a new asset for the player
			std::vector<std::pair<int,std::string>> m_scores;

			fs::path p = "assets/Player/";
			for(const auto& file : fs::directory_iterator(p))
			{
				std::cout << file<<std::endl;
				m_scores.emplace_back(std::make_pair(0,file.path().string()));
			}

			//search for best available picture
			for(const auto& item : m_inventory)
			{
				for(auto& score : m_scores )
				{
					if(score.second.find(item.id) != std::string::npos) score.first++;
				}
			}

			//get the best one
			const std::string bestFile = std::max_element(m_scores.begin(), m_scores.end(), [](auto a,auto b) { return a.first < b.first; })->second;

			m_currentTexture = LoadBufferedImage(bestFile);
			m_invDirty = false;
		}
	}

	//Add a Item to the Inventory
	void addToInventory(Item item)
	{

		//No duplicates! picking up an already present just replenishes the stamina
		for(auto& item_ : m_inventory)
		{
			if(item_.name == item.name)
			{
				item_.num_uses = item.num_uses;
				return;
			}
		}
		m_invDirty = true;
		m_inventory.push_back(item);
	}

	sf::Texture* getTexture()
	{
		return m_currentTexture;
	}


	sf::Font getFont() const
	{
		return gui.fnt;
	}
private:

	sf::Texture* m_currentTexture;


	bool m_invDirty = true;

	const int BASE_ATK = 10;
	const float BASE_DEF = 1;

	int m_health = 100;
	int m_points = 0;

	std::vector<Item> m_inventory;
	

	sf::Clock m_timer;

	struct
	{
		
		sf::Text healthText;
		sf::RectangleShape healthBar;
		sf::RectangleShape healthEmptyBar;

		sf::Text pointsText;
		sf::Text pointsAmountText;

		sf::Text elapsedTime;

		sf::Text inventoryText;
		sf::Text inventoryContentsText;

		sf::Font fnt;
	}gui;

	
	int m_x,m_y;
	gameboard* m_board;
};
