#pragma once
#include <string>
#include "Item.h"
#include <vector>
#include "LevelLoader.h"
#include <nlohmann/json.hpp>

const std::string enemiesTemplate = "level%Enemies.data";

namespace nj = nlohmann;

struct Enemy
{
	int x,y;
	int atk;
	int def;
	int value;
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
	}

	void loadEnemies(int level)
	{
		std::cout << "Loading Level..."<<std::endl;

		const std::string fn = enemiesTemplate.substr(0,enemiesTemplate.find('%'))+std::to_string(level)+enemiesTemplate.substr(enemiesTemplate.find('%')+1);

		std::ifstream ifs(fn);

		if(ifs.bad()) throw std::runtime_error("Enemyfile not found!");
		
		nj::json j;
		ifs >> j;
	}

private:
	gameboard* m_board;

};
