#include "BlockRegistry.h"

void BlockRegistry::loadBlocks(TextureManager &textureManager)
{
	JSONParser::parseBlocks(this->blockTypes, this->blockIDmap, textureManager);
}

BlockRegistry::BlockRegistry()
{
	blockTypes = std::vector<BlockType>();
	//Nullblock for air
}