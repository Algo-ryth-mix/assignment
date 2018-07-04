#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <map>

inline sf::Texture* LoadBufferedImage(std::string filename)
{
	static std::map<std::string,sf::Texture> textures;

	if(textures.find(filename) == textures.end())
	{
		textures.emplace(std::make_pair(filename,sf::Texture()));
		textures[filename].loadFromFile(filename);
	}

	return &textures[filename];
}
