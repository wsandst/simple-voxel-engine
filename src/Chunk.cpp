#include "Chunk.h"

const BlockData& Chunk::getBlockUnsafe(unsigned int x, unsigned int y, unsigned int z) const
{
	return blocks[z * chunkSize * chunkSize + y * chunkSize + x];
}

const BlockData& Chunk::getBlock(int x, int y, int z) const
{
	if (x >= 0 && x <= 31 && y >= 0 && y <= 31 && z >= 0 && z <= 31)
	{
		return blocks[z * chunkSize * chunkSize + y * chunkSize + x];
	}
	else //Return from the adjacent chunk!
	{
		return getNeighbourBlock(x, y, z);
	}
}

const BlockData& Chunk::getBlockByIndex(unsigned int index) const
{
	return blocks[index];
}

BlockData air = BlockData(0, 31, 0);

const BlockData& Chunk::getNeighbourBlock(int x, int y, int z) const
{
	if (x < 0 && chunkNeighbours[1])
		return chunkNeighbours[1]->getBlock(31, y, z);
	else if (x > 31 && chunkNeighbours[0])
		return chunkNeighbours[0]->getBlock(0, y, z);
	else if (y < 0 && chunkNeighbours[3])
		return chunkNeighbours[3]->getBlock(x, 31, z);
	else if (y > 31 && chunkNeighbours[2])
		return chunkNeighbours[2]->getBlock(x, 0, z);
	else if (z < 0 && chunkNeighbours[5])
		return chunkNeighbours[5]->getBlock(x, y, 31);
	else if (z > 31 && chunkNeighbours[4])
		return chunkNeighbours[4]->getBlock(x, y, 0);
	else
		return air;
}


void Chunk::setBlock(BlockData block, unsigned int x, unsigned int y, unsigned int z)
{
	blocks[z*chunkSize*chunkSize + y * chunkSize + x] = block;
}

Chunk::Chunk(int x, int y, int z, GenerationType genType)
{
	const int fillID = 2;
	const int blockRange = 1;
	this->x = x;
	this->y = y;
	this->z = z;
	switch (genType)
	{
	case FILL:
		ChunkGenerator::fill(*this, fillID);
		break;
	case PERLIN3D:
		ChunkGenerator::perlinNoise(*this, fillID, 0.1);
		break;
	case RANDOM:
	{
		ChunkGenerator::random(*this, blockRange, 10);
		break;
	}
	case PERLIN2D:
	{
		ChunkGenerator::heightmap(*this, fillID, 48);
		break;
	}
	}
}

Chunk::Chunk()
{
}


Chunk::~Chunk()
{
	//delete[] blocks;
}
