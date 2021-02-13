#include "ChunkGenerator.h"
#include "FastNoiseSIMD.h"

void ChunkGenerator::fill(Chunk& chunk, int fillID)
{
	std::fill(&chunk.blocks[0], &chunk.blocks[0] + chunk.chunkSize * chunk.chunkSize * chunk.chunkSize, BlockData(2,0,0));
	//chunk.setBlock(BlockData(0, 31, 0), 0, 0, 0);
	//chunk.setBlock(BlockData(1, 0, 0), 6, 0, 5);
}

void ChunkGenerator::perlinNoise(Chunk& chunk, int fillID, float limit)
{
	FastNoiseSIMD* myNoise = FastNoiseSIMD::NewFastNoiseSIMD();

	// Get a set of 32 x 32 x 32 Simplex Fractal noise at Chunk Pos
	float* noiseSet = myNoise->GetSimplexFractalSet(chunk.z*32, chunk.y*32, chunk.x*32, 32, 32, 32);
	int index = 0;

	for (int z = 0; z < 32; z++)
	{
		for (int y = 0; y < 32; y++)
		{
			for (int x = 0; x < 32; x++)
			{
				chunk.blocks[index] = (noiseSet[index] > 0.1) ? BlockData(2,0,0) : BlockData(0, 31, 0);
				index++;
			}
		}
	}

	FastNoiseSIMD::FreeNoiseSet(noiseSet);
}

void ChunkGenerator::heightmap(Chunk& chunk, int fillID, float scale)
{
	int chunkHeight = chunk.y * 32;
	int topLayerHeight = 2;

	const BlockData topBlock = BlockData(fillID, 0, 0);
	const BlockData fillBlock = BlockData(3, 0, 0);
	const BlockData airBlock = BlockData(0, 31, 0);

	//Check whether chunk can be anything but fillblock or air
	if (chunkHeight < 0)
	{
		std::fill(&chunk.blocks[0], &chunk.blocks[0] + chunk.chunkSize * chunk.chunkSize * chunk.chunkSize, fillBlock);
		return;
	}
	else if (chunkHeight > scale)
	{
		std::fill(&chunk.blocks[0], &chunk.blocks[0] + chunk.chunkSize * chunk.chunkSize * chunk.chunkSize, airBlock);
		chunk.isAir = true;
		return;
	}
	std::fill(&chunk.blocks[0], &chunk.blocks[0] + chunk.chunkSize * chunk.chunkSize * chunk.chunkSize, fillBlock);

	FastNoiseSIMD* myNoise = FastNoiseSIMD::NewFastNoiseSIMD();
	float* noiseSet = myNoise->GetSimplexFractalSet(chunk.z * 32, chunk.x * 32, 0, 32, 32, 1);
	int height = 0, fillHeight, noiseIndex = 0;

	for (int z = 0; z < 32; z++)
	{
		for (int x = 0; x < 32; x++)
		{
			//Fill column up to height
			height = (noiseSet[noiseIndex] + 0.5) * scale + topLayerHeight;
			if (height > 20)
			{
				height = std::max(height-5, 20);
			}

			if (height > chunkHeight)
			{
				if (height < (chunkHeight + 33))
					fillHeight = height - chunkHeight;
				else
				{
					fillHeight = 32;
				}
			}
			else
			{
				fillHeight = 0;
			}


			for (int y = fillHeight; y < 32; y++)
			{
				chunk.setBlock(airBlock, x, y, z);
			}

			for (int y = std::max(fillHeight - topLayerHeight,0); y < fillHeight; y++)
			{
				chunk.setBlock(topBlock, x, y, z);
			}

			noiseIndex += 1;
		}
	}

	FastNoiseSIMD::FreeNoiseSet(noiseSet);
}


void ChunkGenerator::random(Chunk& chunk, int range, int prob)
{
		//Random blocks in chunk! Looks pretty cool

		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<> dis(1, prob*range);
		int randNum;
		for (size_t i = 0; i < 32 * 32 * 32; i++)
		{
			randNum = dis(rng);
			if (randNum > range)
			{
				chunk.blocks[i] = 0;
			}
			else
			{
				chunk.blocks[i] = randNum;
			}
		}
		

}

//Static class, never used
ChunkGenerator::ChunkGenerator()
{
}

ChunkGenerator::~ChunkGenerator()
{
}
