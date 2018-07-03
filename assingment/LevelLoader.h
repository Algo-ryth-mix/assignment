#pragma once
#include "Tiles.h"
#include <fstream>
#include <streambuf>
#include <iostream>

const std::string levelTemplate = "level%.data";


class LevelLoader
{
public:
	static void loadLevel(gameboard * board,int l_num)
	{
		
		std::cout << "Loading Level..."<<std::endl;

		const std::string fn = levelTemplate.substr(0,levelTemplate.find('%'))+std::to_string(l_num)+levelTemplate.substr(levelTemplate.find('%')+1);

		std::ifstream file(fn);

		if(file.bad()) throw std::runtime_error("Levelfile not found!");

		std::string contents((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
		file.close();

		int file_iterator = 0;

		std::cout << "[";
		for(int x = 0;x < 32;x++)
		{
			std::cout << "#";
			for(int y = 0;y<32;)
			{
				switch (contents[file_iterator++])
				{
				case '#':
					board->at(y).at(x).state = IS_WALL;
					y++;
					break;
				case '.':
					board->at(y).at(x).state = IS_GROUND;
					y++;
					break;
				case '%':
					board->at(y).at(x).state = IS_WORLD_SPAWN;
					y++;
					break;
				case '$':
					board->at(y).at(x).state = IS_ENEMY;
					y++;
					break;
				default: 
					if(isdigit(contents[file_iterator-1]))
					{
						board->at(y).at(x).state = IS_PORTAL;
						board->at(y).at(x).extraData = static_cast<int>(contents[file_iterator - 1]-'0') ;
						y++;
					}
					
				}
			}
		}
		std::cout << "] Done"<<std::endl;
	}
};
