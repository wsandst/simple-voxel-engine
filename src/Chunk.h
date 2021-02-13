#pragma once
#include "BlockData.h"
#include "ChunkGenerator.h"

#include <algorithm>
#include <random>

enum GenerationType { FILL, PERLIN3D, PERLIN2D, RANDOM };

class ChunkGenerator;

class Chunk
{
public:
	const static int chunkSize = 32;
	int x, y, z;
	BlockData blocks[chunkSize * chunkSize * chunkSize];

	Chunk* chunkNeighbours[6]{};
	bool isAir = false;

	const BlockData& getBlockUnsafe(unsigned int x, unsigned int y, unsigned int z) const;
	const BlockData& getBlock(int x, int y, int z) const;
	const BlockData& getBlockByIndex(unsigned int index) const;
	const BlockData& getNeighbourBlock(int x, int y, int z) const;
	void setBlock(BlockData block, unsigned int x, unsigned int y, unsigned int z);
	Chunk(int x, int y, int z, GenerationType genType);
	Chunk();
	~Chunk();
};

