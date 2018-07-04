#pragma once
#include <string>
#include "Item.h"
#include <vector>
#include "LevelLoader.h"
#include <nlohmann/json.hpp>
#include "ItemLoader.h"

const std::string enemiesTemplate = "level%Enemies.data";

namespace nj = nlohmann;

struct Enemy
{
	std::string name;
	int x,y;
	int atk;
	int def;
	int value;
	int health;
	std::vector<Item> drops;
};


class EnemyController
{
	EnemyController() = default;
public:
	static EnemyController& get()
	{
		static EnemyController e;
		return e;
	}

	void registerGameBoard(gameboard* board)
	{
		m_board = board;
		srand(time(NULL));
	}


	Enemy& getEnemyAt(int x,int y)
	{
		for(Enemy& e : m_enemies)
		{
			if(e.x == x && e.y == y) return e;
		}
		throw std::runtime_error("missmatch with enemy coordinates! check your level file!");
    }
	void removeEnemyAt(int x,int y)
	{
		m_enemies.erase(std::remove_if(m_enemies.begin(),m_enemies.end(),[&](Enemy& e)
		{
			return e.x == x && e.y == y;
		}));
		std::cout << "removed Enemy @:" << x << "-" << y<<std::endl;
	}

	void loadEnemies(int level)
	{

		m_enemies.clear();
		std::cout << "Loading Enemies..."<<std::endl;

		const std::string fn = enemiesTemplate.substr(0,enemiesTemplate.find('%'))+std::to_string(level)+enemiesTemplate.substr(enemiesTemplate.find('%')+1);

		std::ifstream ifs(fn);

		if(ifs.bad()) throw std::runtime_error("Enemyfile not found!");
		
		nj::json j;
		ifs >> j;


		//register all Tiles that are enemies
		for_all_tiles(m_board,[&](int x,int y,Tile& tile)
		{
			if(tile.state == IS_ENEMY)
			{
				m_enemies.emplace_back();
				Enemy &e = m_enemies.back();
				
				std::string id = std::to_string(x)+"-"+std::to_string(y);
				std::cout << "Levelfile has Enemy @:" + id <<std::endl;
				e.name = j[id]["pretty-name"].get<std::string>();

				std::cout << "Enemyfile has Enemy with name \"" << e.name << "\" for this location!"<< std::endl;

				e.x = x;
				e.y = y;
				e.atk = j[id]["stats"]["atk"];
				e.def = j[id]["stats"]["def"];
				e.value = j[id]["stats"]["value"];
				e.health = j[id]["stats"]["health"];
				for(auto& j_obj :j[id]["drops"].get<nj::json::object_t>())
				{
					float probability = j_obj.second.get<float>();
					float random_value = static_cast<float>(rand() % 101) / 100.0f;
					if(random_value < probability)
					{
						e.drops.push_back(ItemLoader::loadItem(j_obj.first));
					}
				}
			}
		});
	}

private:
	gameboard* m_board;
	std::vector<Enemy> m_enemies;

};
