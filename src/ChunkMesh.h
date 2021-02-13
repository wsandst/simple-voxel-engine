#pragma once
#define GLEW_STATIC 
#include "GL/glew.h"

#include "SDL.h"
#include "SDL_opengl.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Chunk.h"
#include "Helpers.h"

#include <iostream>

#include <vector>
#include <unordered_set>

#pragma pack(push, 0)

struct StandardVertexAttrib {
	StandardVertexAttrib(unsigned char x, unsigned char y, unsigned char z, unsigned char u, unsigned char v, unsigned char yOffset, unsigned char xOffset, unsigned char light) {
		position[0] = x;
		position[1] = y;
		position[2] = z;

		quadDim[0] = u;
		quadDim[1] = v;

		textureOffsets[0] = yOffset;
		textureOffsets[1] = xOffset;

		lightLevel = light;
	}

	unsigned char position[3];
	unsigned char quadDim[2];
	unsigned char textureOffsets[2];
	unsigned char lightLevel;
};

#pragma pack(pop)

struct CustomVertexAttrib {
	CustomVertexAttrib(float x, float y, float z, float u, float v) {
		position[0] = x;
		position[1] = y;
		position[2] = z;

		uv[0] = u;
		uv[1] = v;
	}

	float position[3];
	float uv[2];
};


class ChunkMesh
{
public:
	Vec3i chunkPos;
	bool hasStandardBlocks;
	bool hasTransparentBlocks;
	bool hasCustomBlocks;

	std::vector<StandardVertexAttrib> standardVertices;
	std::vector<StandardVertexAttrib> transparentVertices;
	std::vector<CustomVertexAttrib> customVertices;
	ChunkMesh(Vec3i chunkPos, std::vector<StandardVertexAttrib> &standardVertices, std::vector<StandardVertexAttrib> &transparentVertices, std::vector<CustomVertexAttrib> &customVertices)
	{
		this->standardVertices = standardVertices;
		this->transparentVertices = transparentVertices;
		this->customVertices = customVertices;

		hasStandardBlocks = (standardVertices.size() != 0);
		hasTransparentBlocks = (transparentVertices.size() != 0);
		hasCustomBlocks = (customVertices.size() != 0);

		this->chunkPos = chunkPos;
	}
	ChunkMesh() {};
	~ChunkMesh() {};
};

