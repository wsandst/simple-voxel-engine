#pragma once
#include "Chunk.h"

#include "FastNoiseSIMD.h"
#include <iostream>

class Chunk;

class ChunkGenerator
{
public:
	static void fill(Chunk& chunk, int fillID);
	static void perlinNoise(Chunk& chunk, int fillID, float limit);
	static void heightmap(Chunk& chunk, int fillID, float scale);
	static void random(Chunk& chunk, int range, int prob);
private:
	ChunkGenerator();
	~ChunkGenerator();
};

