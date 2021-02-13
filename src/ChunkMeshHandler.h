#pragma once
#include "Chunk.h"
#include "ChunkMesh.h"
#include "BlockType.h"
#include "Helpers.h"

#include <ctime>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <queue>


class ChunkMeshHandler
{
public:
	std::queue<ChunkMesh*> newChunkMeshes;
	std::queue<Vec3i> chunkUnloads;
	std::vector<BlockType> blockTypes;

	float textureOffset;
	~ChunkMeshHandler();
	void addChunk(Chunk& chunk);
	void removeChunk(Chunk& chunk);
	void registerBlockType(BlockType newType);
	
	ChunkMeshHandler(int maxBlockTypes);
	ChunkMeshHandler();
private:
	const bool SMOOTH_LIGHTING = true;

	struct FaceData
	{
		bool isNull = false;
		StandardTextureUV texture;
		bool transparent;
		unsigned char light[4];
		const bool operator==(const FaceData& rhs) //No comparison on transparent, doesn't really make sense
		{
			return (texture.x == rhs.texture.x && texture.y == rhs.texture.y
				&& light[0] == rhs.light[0] && light[1] == rhs.light[1]
				&& light[2] == rhs.light[2] && light[3] == rhs.light[3]);
		}
		const bool operator!=(const FaceData& rhs)
		{
			return !operator==(rhs);
		}
		FaceData(StandardTextureUV texture, bool transparent, unsigned char light)
		{
			this->texture = texture;
			this->transparent = transparent;
			this->light[0] = light;
			this->light[1] = light;
			this->light[2] = light;
			this->light[3] = light;
		}
		FaceData(StandardTextureUV texture, bool transparent, unsigned char light1, unsigned char light2, unsigned char light3, unsigned char light4)
		{
			this->texture = texture;
			this->transparent = transparent;
			this->light[0] = light1;
			this->light[1] = light2;
			this->light[2] = light3;
			this->light[3] = light4;
		}
		FaceData() //Uninitialized is null
		{
			this->isNull = true;
		}
	};
	FaceData greedyMask[32*32];

	void clearGreedyMask();
	FaceData getFaceData(const Chunk& chunk, const int& x, const int& y, const int& z, const BlockFace& side);
	void createQuad(std::vector<StandardVertexAttrib>& vertices, const Vec3i& bottomLeft, const Vec3i& bottomRight, const Vec3i& topLeft, const Vec3i& topRight, int w, int h, const FaceData& face, const BlockFace& side);
	void getVertexLight(const Chunk& chunk, const bool transparet, const int& x, const int& y, const int& z, const BlockFace& side, unsigned char& light1, unsigned char& light2, unsigned char& light3, unsigned char& light4, bool& visible);
	void generateGreedyMesh(std::vector<StandardVertexAttrib>& standardVertices, std::vector<StandardVertexAttrib>& transparentVertices, const Chunk& chunk);
	//void generateChunkMesh(std::vector<StandardVertexAttrib>& standardVertices, std::vector<StandardVertexAttrib>& transparentVertices, std::vector<CustomVertexAttrib>& customVertices, Chunk& chunk);
	//void meshDefaultBlock(std::vector<StandardVertexAttrib>& mesh, BlockFace face, unsigned char startX, unsigned char x, unsigned char y, unsigned char z, TextureInfo & textureInfo);
	//void meshNonDefaultBlock(std::vector<StandardVertexAttrib>& mesh, int blockID, BlockModelType blockModel, unsigned char x, unsigned char y, unsigned char z);
	//void meshCustomBlock(std::vector<CustomVertexAttrib>& mesh, int blockID, unsigned char x, unsigned char y, unsigned char z, TextureUV uv);

	inline bool isBlockAir(int index, Chunk & chunk);
};