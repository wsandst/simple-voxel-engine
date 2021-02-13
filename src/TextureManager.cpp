#include "TextureManager.h"


void TextureManager::generateBlockAtlas(std::vector<BlockType>& blockTypes)
{
	TextureAtlasSquare& atlas = blockAtlas;
	unsigned char x, y;
	for (auto& it : standardTexToIDmap) {
		// Do stuff
		std::string name = it.first;
		Texture texture = FileHandler::loadImage(name, atlas.colorChannels);

		x = blockAtlas.x;
		y = blockAtlas.y;

		for (size_t i = 0; i < it.second.size(); i++) //Link the blockTypes to the corresponding texture
		{
			switch (it.second[i].first)
			{
			case POSX:
			{
				blockTypes[it.second[i].second].textures.back = StandardTextureUV{ x, y };
				break;
			}
			case POSY:
			{
				blockTypes[it.second[i].second].textures.top = StandardTextureUV{ x, y };
				break;
			}
			case POSZ:
			{
				blockTypes[it.second[i].second].textures.right = StandardTextureUV{ x, y };
				break;
			}
			case NEGX:
			{
				blockTypes[it.second[i].second].textures.front = StandardTextureUV{ x, y };
				break;
			}
			case NEGY:
			{
				blockTypes[it.second[i].second].textures.bottom = StandardTextureUV{ x, y };
				break;
			}
			case NEGZ:
			{
				blockTypes[it.second[i].second].textures.left = StandardTextureUV{ x, y };
				break;
			}
			}
		}
		atlas.blit(texture.data);
	}
	//Output the atlas as a file for testing purposes
	stbi_write_png("atlas.png", 2048, 2048, atlas.colorChannels, atlas.atlas.data(), 2048 * atlas.colorChannels);
}

void TextureManager::generateCustomAtlas(std::vector<BlockType>& blockTypes)
{
	TextureAtlas& atlas = customAtlas;
	unsigned char x, y;
	for (auto& it : customTexToIDmap) {
		// Do stuff
		std::string name = it.first;
		Texture texture = FileHandler::loadImage(name, atlas.colorChannels);
		atlas.addRect(texture.width, texture.height, texture.data, it.second);
	}
	atlas.generateAtlas();
	for (size_t i = 0; i < atlas.rects.size(); i++)
	{
		int id = atlas.rects[i].id;
		float x = atlas.rects[i].x / float(atlas.maxTextureSize);
		float y = atlas.rects[i].y / float(atlas.maxTextureSize);
		float w = atlas.rects[i].w / float(atlas.maxTextureSize);
		float h = atlas.rects[i].h / float(atlas.maxTextureSize);
		for (size_t j = 0; j < atlas.blockIDs[id].size(); j++)
		{
			blockTypes[atlas.blockIDs[id][j]].customTexture = TextureUV{ x,y,w,h };
		}
	}
	atlas.cleanup();
	//Output the atlas as a file for testing purposes
	stbi_write_png("customAtlas.png", 2048, 2048, atlas.colorChannels, atlas.atlas.data(), 2048 * atlas.colorChannels);
}

TextureManager::TextureManager()
{
	blockAtlas = TextureAtlasSquare(2048, 4);
	customAtlas = TextureAtlas(2048, 4);
}


TextureManager::~TextureManager()
{
}

void TextureManager::insertTextureToMap(std::string textureName, int blockID, BlockFace blockFace)
{
	if (standardTexToIDmap.find(textureName) == standardTexToIDmap.end()) //Texture not already loaded
	{
		standardTexToIDmap[textureName] = std::vector<std::pair<BlockFace, int>>();
	}
	standardTexToIDmap[textureName].emplace_back(blockFace, blockID);
}

void TextureManager::insertTextureToCustomMap(std::string textureName, int blockID)
{
	if (customTexToIDmap.find(textureName) == customTexToIDmap.end()) //Texture not already loaded
	{
		customTexToIDmap[textureName] = std::vector<int>();
	}
	customTexToIDmap[textureName].emplace_back(blockID);
}