#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>
#include <array>

const float TILE_SIZE = 25.0f;

enum TileState
{
	IS_WORLD_SPAWN,
	IS_PLAYER,
	IS_GROUND,
	IS_WALL,
	IS_PORTAL,
	IS_ENEMY
};


struct Tile
{
	TileState state;
	sf::RectangleShape repr;
	int extraData = -1;
};


typedef std::array<std::array<Tile,32>,32> gameboard;

const void for_all_tiles(gameboard* b,std::function<void(int x,int y,Tile &tile)> func)
{
	for(int x = 0;x <32;x++)
	{
		for(int y = 0;y< 32;y++)
		{
			auto& tile = b->at(x).at(y);
			func(x,y,tile);
		}
	}
}