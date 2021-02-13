#pragma once
#include "JSONParser.h"
#include "BlockType.h"

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

class BlockRegistry{
public:

	static const int maxBlockTypes = 32;
	std::vector<BlockType> blockTypes;
	std::unordered_map<std::string, int> blockIDmap;
	void loadBlocks(TextureManager& textureManager);

	BlockRegistry();
	
private:

};