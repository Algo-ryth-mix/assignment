#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include <array>
#include "Tiles.h"
#include "Player.h"
#include "LevelLoader.h"
#include "ItemLoader.h"
#include "EnemyController.h"


int main(int argc,char ** argv)
{
	//Player Texture
	sf::Texture wallTex;
	wallTex.loadFromFile("assets/Environment/Wall.png");

	sf::Texture portalTex;
	portalTex.loadFromFile("assets/Environment/Portal.png");

	//create window
	sf::RenderWindow m_window(sf::VideoMode(1000,800),"Time Trial | Almost Roguelike - Raphael Baier!");
	
	//create playfield
	gameboard tiles;
	
	//load level 1 (make sure level1.data exists)
	LevelLoader::loadLevel(&tiles,1);

	EnemyController& controller = EnemyController::get();

	controller.registerGameBoard(&tiles);
	controller.loadEnemies(1);

	//create initial draw information
	for_all_tiles(&tiles,[](int x,int y,Tile& t){
		t.repr.setSize({TILE_SIZE,TILE_SIZE});
		t.repr.setPosition({TILE_SIZE*x,TILE_SIZE*y});
	});
	
	//create player
	Player player(&tiles);
	player.addToInventory(ItemLoader::loadItem("armor-iron"));
	

	//game loop
	while(m_window.isOpen())
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
				if(event.key.code == sf::Keyboard::W)
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
					LevelLoader::loadLevel(&tiles,1);
					player.spawn();
				}
			}
		}

		m_window.clear(sf::Color::Black);

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
					tile.repr.setFillColor(sf::Color::White);
			}break;
			case IS_WALL:
			{
					tile.repr.setTexture(&wallTex);
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


	return 0;

}
