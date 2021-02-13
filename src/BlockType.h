#pragma once
#include <string>
#include "BlockFace.h"
#include <map>
#include <vector>
#include "Models.h"

struct StandardTextureUV
{
	unsigned char x, y;
};

struct TextureUV
{
	float x, y, w, h;
};

struct TextureInfo
{
	StandardTextureUV front;
	StandardTextureUV back;
	StandardTextureUV top;
	StandardTextureUV bottom;
	StandardTextureUV left;
	StandardTextureUV right;

	//For custom models which use the custom VBO
	StandardTextureUV custom;

	TextureInfo() {}

	StandardTextureUV getSide(const BlockFace side) const
	{
		switch (side)
		{
		case LEFT:
			return left;
		case RIGHT:
			return right;
		case FRONT:
			return front;
		case BACK:
			return back;
		case BOTTOM:
			return bottom;
		case TOP:
			return top;
		}
	}

	TextureInfo(StandardTextureUV singleLayer)
	{
		this->front = singleLayer;
		this->back = singleLayer;
		this->top = singleLayer;
		this->bottom = singleLayer;
		this->left = singleLayer;
		this->right = singleLayer;
		this->custom = singleLayer;
	}

	TextureInfo(StandardTextureUV front, StandardTextureUV back, StandardTextureUV top, StandardTextureUV bottom, StandardTextureUV right, StandardTextureUV left)
	{
		this->front = front;
		this->back = back;
		this->top = top;
		this->bottom = bottom;
		this->left = left;
		this->right = right;
	}
};

struct CustomTextureInfo
{
	int x, y, w, h;
};

class BlockType
{
public:
	//Contains the information for a certain blockID
	//This information is basically a copy of what is in the JSON file
	//But with some differences, like texture layer instead of path
	//And it should contain the vertices
	unsigned int blockID;
	std::string name;

	bool isTransparent = false; //Glass, leaves etc
	bool isNonSolid = false; //Slab, slope etc
	BlockModelType blockModelType = BLOCK;

	std::vector<float> customModel;
	TextureInfo textures;
	TextureUV customTexture;

	BlockType(std::string name, int blockID)
	{
		this->name = name;
		this->blockID = blockID;
	}

	BlockType() {}
};
