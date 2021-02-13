#pragma once

#include <vector>
#include <iostream>
#include <cstring>

//For packing the atlas
#include "stb_rect_pack.h"


struct TextureAtlasSquare //Texture atlas for square textures with same resolution, for standard and transparent VBO
{
	TextureAtlasSquare(int maxTextureSize, int colorChannels = 3)
	{
		atlas = std::vector<unsigned char>(maxTextureSize*maxTextureSize * colorChannels);
		this->maxTextureSize = maxTextureSize;
		maxTexturesPerDim = maxTextureSize / textureRes;
		textureOffset = 1.0f / maxTexturesPerDim;
		this->colorChannels = colorChannels;
	}
	TextureAtlasSquare()
	{

	}
	~TextureAtlasSquare()
	{

	}
	void blit(unsigned char* texture)
	{
		int yOffset = this->y*textureRes;
		const int bytes = colorChannels;
		for (size_t y = 0; y < textureRes; y++)
		{
			int atlasIndex = (yOffset + y)*maxTextureSize * bytes + this->x*textureRes * bytes;
			int textureIndex = y * textureRes * bytes;
			std::memcpy(&atlas[atlasIndex], &texture[textureIndex], textureRes * bytes);
		}
		int maxTexturesInX = maxTextureSize / textureRes;
		x = (x + 1) % maxTexturesPerDim;
	}
	float getAtlasX()
	{
		return x / float(maxTextureSize);
	}
	float getAtlasY()
	{
		return y / float(maxTextureSize);
	}
	std::vector<unsigned char> atlas;
	unsigned int maxTextureSize = 2048;
	int textureRes = 16;
	int maxTexturesPerDim = 128;
	int x = 0, y = 0;
	float textureOffset;
	int colorChannels = 3;
};

struct TextureAtlas //Texture atlas for rectangular textures, for custom vbo
{
	std::vector<unsigned char> atlas;
	std::vector<stbrp_rect> rects;
	std::vector<unsigned char*> textures;
	std::vector<std::vector<int>> blockIDs;
	unsigned int maxTextureSize = 2048;
	int colorChannels = 3;
	int id = 0;

	TextureAtlas(int maxTextureSize, int colorChannels = 3)
	{
		atlas = std::vector<unsigned char>(maxTextureSize * maxTextureSize * colorChannels);
		this->colorChannels = colorChannels;
		std::vector<unsigned char*> textures = std::vector<unsigned char*>();
		std::vector<stbrp_rect> rects = std::vector<stbrp_rect>();
		std::vector<std::vector<int>> blockIDs = std::vector<std::vector<int>>();
	}
	TextureAtlas()
	{
	}
	~TextureAtlas()
	{

	}
	void generateAtlas()
	{
		//STB_RECTPACK setup
		stbrp_context context;
		int nodeCount = maxTextureSize * maxTextureSize;
		std::vector<stbrp_node> nodes = std::vector<stbrp_node>(nodeCount);
		//Pack the textures
		if (rects.size() > 0)
		{
			stbrp_init_target(&context, 4096, 4096, &nodes[0], nodeCount);
			stbrp_pack_rects(&context, &rects[0], rects.size());
		}

		for (size_t i = 0; i < rects.size(); i++)
		{
			if (rects[i].was_packed > 0) //Packed correctly, insert
			{
				blit(textures[rects[i].id], rects[i].x, rects[i].y, rects[i].w, rects[i].h);
			}
			else
			{
				std::cout << "ERROR: Texture of id " << rects[i].id << " was not packed correctly into atlas";
			}
		}
		std::cout << "Finished packing of texture atlas" << std::endl;
	}

	void cleanup()
	{
		rects.clear();
		textures.clear();
		blockIDs.clear();
	}

	void addRect(int width, int height, unsigned char* texture, std::vector<int> blockIDs)
	{
		stbrp_rect rect;
		rect.h = height;
		rect.w = width;
		rect.x = 0;
		rect.y = 0;
		rect.id = id;
		rect.was_packed = 0;
		rects.push_back(rect);
		textures.push_back(texture);
		this->blockIDs.push_back(blockIDs);
		id++;
	}
	
	void blit(unsigned char* texture, int ix, int iy, int w, int h)
	{
		const int bytes = colorChannels;
		for (size_t y = 0; y < h; y++)
		{
			int atlasIndex = (iy + y)*maxTextureSize * bytes + ix * bytes;
			int textureIndex = y * w * bytes;
			std::memcpy(&atlas[atlasIndex], &texture[textureIndex], w * bytes);
		}
	}
};
