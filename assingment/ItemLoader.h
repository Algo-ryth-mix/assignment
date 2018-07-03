#pragma once
#include "Item.h"
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
namespace nj = nlohmann;
class ItemLoader
{
public:	
	static Item loadItem(std::string name)
	{
		std::ifstream ifs("items.data");
		if(ifs.bad()) throw std::runtime_error("cannot get my Items file!");
		nj::json j;
		ifs >> j;

		Item item{};

		item.name = j[name]["pretty-name"].get<std::string>();
		item.num_uses = j[name]["uses"].get<int>();
		if(!j[name]["def"].is_null())
		{
			item.def_buf = j[name]["def"].get<int>();
		}
		else item.def_buf = 0;
		
		if(!j[name]["atk"].is_null())
		{
			item.atk_buf = j[name]["atk"].get<int>();
		}
		else item.atk_buf = 0;

		return item;
	}
};
