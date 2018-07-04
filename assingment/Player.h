#pragma once

#include "Tiles.h"
#include "LevelLoader.h"
#include "Item.h"


class Player
{
public:
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

	sf::Vector2i getPosition() const
	{
		return {m_x,m_y};
	}

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

			//respawn in level
			spawn();
			return;
		}

		//handle portals
		if(m_board->at(x).at(y).state == IS_ENEMY)
		{
			m_health-=10;
			m_points+=5;
		}

		//"swap" places with the tile
		m_board->at(m_x)	.at(m_y)	.state = IS_GROUND;
		m_board->at(x)		.at(y)		.state = IS_PLAYER;
		m_x = x;
		m_y = y;
	}

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

	void update()
	{
		//did the player take longer than 10 seconds to complete the level ?
		if(m_timer.getElapsedTime().asMilliseconds() > 10000)
		{
			m_health = 0; //yes level failed
		}

		gui.elapsedTime.setString(std::to_string(m_timer.getElapsedTime().asSeconds()).c_str());
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
		
		m_inventory.push_back(item);
	}


private:

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
