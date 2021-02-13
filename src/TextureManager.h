#pragma once
#include "TextureAtlas.h"
#include "FileHandler.h"
#include "BlockFace.h"
#include "BlockType.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include <vector>
#include <unordered_map>

class TextureManager
{
public:
	TextureAtlasSquare blockAtlas;
	TextureAtlas customAtlas;
	TextureAtlas entityAtlas;

	int maxTextureSize = 2048;


	void generateBlockAtlas(std::vector<BlockType>& blockTypes); //Blocktypes are needed to link textures to blockregistry
	void generateCustomAtlas(std::vector<BlockType>& blockTypes);
	TextureManager();
	~TextureManager();

	void insertTextureToMap(std::string textureName, int blockID, BlockFace blockFace);

	void insertTextureToCustomMap(std::string textureName, int blockID);

private:
	std::unordered_map<std::string, std::vector<int>> customTexToIDmap = std::unordered_map<std::string, std::vector<int>>();
	std::unordered_map<std::string, std::vector<std::pair<BlockFace, int>>> standardTexToIDmap = std::unordered_map<std::string, std::vector<std::pair<BlockFace, int>>>();
};

