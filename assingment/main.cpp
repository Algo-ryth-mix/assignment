#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include <array>
#include "Tiles.h"
#include "Player.h"
#include "LevelLoader.h"
#include "ItemLoader.h"
#include "EnemyController.h"


#define __PIFTRUE

#if defined(__PIFTRUE)
#define ONLY_IF(X) X
#else
#define ONLY_IF(X)
#endif


int main(int argc,char ** argv)
{
	try{

	sf::RectangleShape bg;
	bg.setSize({1000,800});
	bg.setPosition({0,0});
	bg.setFillColor(sf::Color::White);
	bg.setTexture(LoadBufferedImage("assets/Environment/BG1.png"));

	//Player Texture
	sf::Texture* wallTex = LoadBufferedImage("assets/Environment/Wall.png");

	sf::Texture portalTex;
	portalTex.loadFromFile("assets/Environment/Portal.png");

	sf::Texture groundTex;
	groundTex.loadFromFile("assets/Environment/Gravel.png");

	//create window
	sf::RenderWindow m_window(sf::VideoMode(1000,800),"Time Trial | Almost Roguelike - Raphael Baier!");
	
	//create playfield
	gameboard tiles;
	
	//load level 1 (make sure level1.data exists)
	LevelLoader::loadLevel(&tiles,0);

	EnemyController& controller = EnemyController::get();

	controller.registerGameBoard(&tiles);
	controller.loadEnemies(0);

	//create initial draw information
	for_all_tiles(&tiles,[](int x,int y,Tile& t){
		t.repr.setSize({TILE_SIZE,TILE_SIZE});
		t.repr.setPosition({TILE_SIZE*x,TILE_SIZE*y});
	});
	
	//create player
	Player player(&tiles);
	//player.addToInventory(ItemLoader::loadItem("armor-iron"));
	

	//game loop
	while(m_window.isOpen() ONLY_IF(&& !player.is_dead()))
	{
		//make our player tick
		player.update();

		//process keyboard events
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
				m_window.close();

			if(event.type == sf::Event::KeyPressed)
			{
				if(event.key.code == sf::Keyboard::Num1)
				{
					bg.setTexture(LoadBufferedImage("assets/Environment/BG1.png"));
				}
				else if(event.key.code == sf::Keyboard::Num2)
				{
					bg.setTexture(LoadBufferedImage("assets/Environment/BG2.png"));
				}
				else if(event.key.code == sf::Keyboard::Num3)
				{
					bg.setTexture(LoadBufferedImage("assets/Environment/BG3.png"));
				}
				else if(event.key.code == sf::Keyboard::Num9)
				{
					
					wallTex = LoadBufferedImage("assets/Environment/Bush.png");
				}
				else if(event.key.code == sf::Keyboard::W)
				{
					player.setPosition(player.getPosition().x,player.getPosition().y-1);
				}
				else if(event.key.code == sf::Keyboard::A)
				{
					player.setPosition(player.getPosition().x-1,player.getPosition().y);
				}
				else if(event.key.code == sf::Keyboard::S)
				{
					player.setPosition(player.getPosition().x,player.getPosition().y+1);
				}
				else if(event.key.code == sf::Keyboard::D)
				{
					player.setPosition(player.getPosition().x+1,player.getPosition().y);
				}
				else if(event.key.code == sf::Keyboard::F5)
				{
					LevelLoader::loadLevel(&tiles,7);
					EnemyController::get().loadEnemies(7);
					player.spawn();
				}
			}
		}

		m_window.clear(sf::Color::White);

		//draw Background first
		m_window.draw(bg);

		//render pass
		for_all_tiles(&tiles,[&](int x,int y,Tile &tile){
			switch (tile.state)
			{
			case IS_PLAYER:
			{
					tile.repr.setTexture(player.getTexture());
					tile.repr.setFillColor(sf::Color::White);
			}break;
			case IS_GROUND:	{
					tile.repr.setTexture(nullptr);
					tile.repr.setFillColor(sf::Color(0,0,0,0));
			}break;
			case IS_WALL:
			{
					tile.repr.setTexture(wallTex);
					
					tile.repr.setFillColor(sf::Color::White);
			}break;
			case IS_PORTAL: 			
			{
					tile.repr.setTexture(&portalTex);
					tile.repr.setFillColor(sf::Color::White);
			}break;
			case IS_ENEMY:
			{
					Enemy& e = EnemyController::get().getEnemyAt(x,y);
					tile.repr.setTexture(LoadBufferedImage("assets/Mobs/" + e.name + ".png"));
					tile.repr.setFillColor(sf::Color::White);break;
			}
			default: ;
			}

			m_window.draw(tile.repr);
		});

		
		player.showGUI(m_window);

		m_window.display();

	}
	sf::Font fnt;
	fnt.loadFromFile("VT323-Regular.ttf");
	sf::Text gameOverText;
	sf::RectangleShape gameOverBar;

	gameOverBar.setSize({800,100});
	gameOverBar.setPosition({0,375});
	gameOverBar.setFillColor(sf::Color::White);

	gameOverText.setFont(fnt);
	gameOverText.setString("Game Over!");
	gameOverText.setCharacterSize(50);
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setPosition(350,390);

	while(m_window.isOpen())
	{
		sf::Event evnt;
		while(m_window.pollEvent(evnt))
		{
			if(evnt.type == sf::Event::Closed)
			{
				m_window.close();
			}
		}

		m_window.draw(gameOverBar);
		m_window.draw(gameOverText);
		m_window.display();
	}
	
	return 0;
	}
	catch(nullptr_t){}
	/*
	catch(nlohmann::detail::exception &e)
	{
		std::cout << e.what();
		throw;
	}
	*/

}
