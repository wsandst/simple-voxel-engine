#pragma once
#include "BlockType.h"
#include "TextureManager.h"
#include "FileHandler.h"

#include "nlohmann/json.hpp"

#include <iostream>
#include <fstream>

using json = nlohmann::json;

//Static parser for JSON
class JSONParser
{
public:
	static void parseBlocks(std::vector<BlockType>& blockTypes, std::unordered_map<std::string, int>& blockIDmap, TextureManager& textureManager);
private:
	static bool loadBlockIDmap(std::unordered_map<std::string, int>& blockIDmap, int &highestID);
	static void saveBlockIDmap(std::unordered_map<std::string, int>& blockIDmap);
	static std::string getBlockModelName(BlockModelType type);
	//Returned value is whether the ID already was registered in the loaded blockIDmap
	static int registerBlockID(std::unordered_map<std::string, int>& blockIDmap, std::string &blockName, int &id);


	JSONParser();
	~JSONParser();
};

