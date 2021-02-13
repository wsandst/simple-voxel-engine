#include "ChunkLoader.h"

void ChunkLoader::generateChunk(Vec3i pos, unsigned int fill)
{
	if (true) {
		clock_t begin = clock();

		chunks[pos] = Chunk(pos.x, pos.y, pos.z, PERLIN2D);

		clock_t end = clock();
		//std::cout << "Chunk gen took " << (double(end - begin) / (CLOCKS_PER_SEC)) * 1000 << "ms \n";
		}
}

void ChunkLoader::meshChunk(Vec3i pos)
{
	if (!chunks[pos].isAir) {
		clock_t begin = clock();

		chunkMeshHandler.addChunk(chunks[pos]);

		clock_t end = clock();

	    //std::cout << "Chunk meshing took " << (double(end - begin) / (CLOCKS_PER_SEC)) * 1000 << "ms \n";
	}
}

void ChunkLoader::chunkLoadingLoop()
{
	//From the current player position
	while (running)
	{
		clock_t begin = clock();
		if (!stopGenerating)
			expandChunkEdges();

		clock_t end = clock();

		//std::cout << "Expanding edges took " << (double(end - begin) / (CLOCKS_PER_SEC)) * 1000 << "ms \n";

		while (!chunkUpdates.empty()) //Load all chunk updates
		{
			Vec3i pos = chunkUpdates.top();
			chunkUpdating(pos);

			chunkUpdates.pop();
		}
		while (!chunkMeshUpdates.empty())
		{
			Vec3i pos = chunkMeshUpdates.top();
			meshChunk(pos);

			chunkMeshUpdates.pop();
		}
		checkEdgeValidity();
	}
}

inline int ChunkLoader::floorDivideDown(int a, int b)
{
	if (a >= 0) return a / b;
	else return (a - b + 1) / b;
}

void ChunkLoader::expandChunkEdges()
{
	std::vector<Chunk*> newChunkEdges = std::vector<Chunk*>();

	bool hasNonExpandedEdge = false;
	int newChunkX, newChunkY, newChunkZ;

	int adjacentPosX, adjacnetPosY, adjacentPosY;
	int selfFromNeighbourIndex;
	int edgeChunkSize = edgeChunks.size();
	for (auto it = edgeChunks.begin(); it != edgeChunks.end();)
	{
		if (!isChunkWithinRenderDistance(*it)) //Unload chunk if outside render distance
		{
			for (size_t j = 0; j < 6; j++) //Go through neighbours of chunk
			{
				if ((*it)->chunkNeighbours[j]) //If not null, chunk exists
				{
					//Remove pointer to self
					selfFromNeighbourIndex = j + 1 - (2 * (j % 2));
					(*it)->chunkNeighbours[j]->chunkNeighbours[selfFromNeighbourIndex] = nullptr;
					newChunkEdges.push_back((*it)->chunkNeighbours[j]); //This is now a new edge chunk
				}
			}
			//Safe to unload
			unloadChunk(Vec3i{ (*it)->x, (*it)->y, (*it)->z });
			it = edgeChunks.erase(it);
			continue;
		}
		else
		{
			it++;
		}
	}
	for (size_t i = 0; i < edgeChunks.size(); i++)
	{
		for (size_t j = 0; j < 6; j++) //Go through edges of edge chunk
		{
			if (!edgeChunks[i]->chunkNeighbours[j]) //Null pointer
			{
				newChunkX = edgeChunks[i]->x + (j == 0) - (j == 1);
				newChunkY = edgeChunks[i]->y + (j == 2) - (j == 3);
				newChunkZ = edgeChunks[i]->z + (j == 4) - (j == 5);
				if (isChunkWithinRenderDistance(newChunkX, newChunkY, newChunkZ)) //Within render distance, add!
				{
					if (newChunkX < -500)
					{
						int x = 5;
					}
					chunkUpdates.push(Vec3i{ newChunkX, newChunkY, newChunkZ });
				}
				else
				{
					hasNonExpandedEdge = true;
				}
			}
		}
		if (hasNonExpandedEdge)
		{
			inactiveEdgeChunks.push_back(edgeChunks[i]);
			hasNonExpandedEdge = false;
		}
	}
	edgeChunks = newChunkEdges;
}

bool ChunkLoader::isChunkWithinRenderDistance(Chunk* chunk)
{
	return (((centerChunkX - chunk->x) * (centerChunkX - chunk->x) +
		(centerChunkY - chunk->y) * (centerChunkY - chunk->y) +
		(centerChunkZ - chunk->z) * (centerChunkZ - chunk->z))
		<= renderDistanceSq);
}

bool ChunkLoader::isChunkWithinRenderDistance(int x, int y, int z)
{
	return (((centerChunkX - x) * (centerChunkX - x) +
		(centerChunkZ - z) * (centerChunkZ - z))
		<= renderDistanceSq && std::abs(y) < (renderDistance/2));
}

void ChunkLoader::updateCenter(int x, int y, int z)
{
	int inputChunkX = floorDivideDown(x , 64);
	int inputChunkY = floorDivideDown(y , 64);
	int inputChunkZ = floorDivideDown(z , 64);
	if (inputChunkX != centerChunkX || inputChunkY != centerChunkY || inputChunkZ != centerChunkZ)
	{
		std::cout << "Updated chunk center: (" << inputChunkX << "," << inputChunkY << "," << inputChunkZ <<  ")" << std::endl;
		centerChunkX = inputChunkX;
		centerChunkY = inputChunkY;
		centerChunkZ = inputChunkZ;
		viewUpdated = true;
	}
}

std::queue<ChunkMesh*>& ChunkLoader::getChunkMeshReference()
{
	return chunkMeshHandler.newChunkMeshes;
}

std::queue<Vec3i>& ChunkLoader::getChunkUnloadReference()
{
	return chunkMeshHandler.chunkUnloads;
}


void ChunkLoader::startChunkLoading()
{
	running = true;
	chunkLoadingThread = std::thread(&ChunkLoader::chunkLoadingLoop, this);
}

void ChunkLoader::stopChunkLoading()
{
	running = false;
	chunkLoadingThread.join();
}

ChunkLoader::ChunkLoader()
{
}


ChunkLoader::~ChunkLoader()
{
}

void ChunkLoader::updateChunk(Vec3i pos)
{
	chunkMeshUpdates.push(pos);
}

void ChunkLoader::chunkUpdating(Vec3i pos)
{
	if (chunks.find(pos) == chunks.end()) //If chunk not generated, generate it
	{
		generateChunk(pos);
		edgeChunks.push_back(&chunks[pos]);
		linkChunkToAdjacent(pos);
	}
}

void ChunkLoader::linkChunkToAdjacent(Vec3i pos)
{
	Chunk& chunk = chunks[pos];
	int adjacentX, adjacentY, adjacentZ;
	int selfFromNeighbourIndex;
	bool hasAllAdjacent = true;
	for (size_t j = 0; j < 6; j++) //Go through edges of edge chunk
	{
		adjacentX = (chunk.x + (j == 0) - (j == 1));
		adjacentY = (chunk.y + (j == 2) - (j == 3));
		adjacentZ = (chunk.z + (j == 4) - (j == 5));
		auto it = chunks.find(Vec3i{ adjacentX, adjacentY, adjacentZ });
		if (it != chunks.end()) //Adjacent chunk is loaded
		{
			selfFromNeighbourIndex = j + 1 - (2 * (j % 2));
			chunk.chunkNeighbours[j] = &it->second;
			it->second.chunkNeighbours[selfFromNeighbourIndex] = &chunk;
			//Check if chunk is full, then mesh
			for (size_t j = 0; j < 6; j++)
			{
				if (!it->second.chunkNeighbours[j])
				{
					hasAllAdjacent = false;
					break;
				}
			}
			//std::cout << it->first.x << " " << it->first.y << " " << it->first.z << std::endl;
			if (hasAllAdjacent && meshedChunks.count(it->first) == 0)
			{
				meshedChunks.insert(it->first);
				chunkMeshUpdates.push(it->first);
			}
			else
			{
				hasAllAdjacent = true;
			}
		}
	}
}

void ChunkLoader::checkEdgeValidity()
{
	if (edgeChunks.size() == 0 && inactiveEdgeChunks.size() == 0) //No chunks loaded, probably new instance! Load in the first chunk
	{
		chunkUpdates.push(Vec3i{ centerChunkX, centerChunkY, centerChunkZ });
	}
}

void ChunkLoader::unloadChunk(Vec3i pos)
{
	chunks.erase(pos);
	meshedChunks.erase(pos);
    chunkMeshHandler.chunkUnloads.push(pos);

}

void ChunkLoader::updateChunkBoundries()
{
	std::cout << "Updating chunk boundraries" << std::endl;
	for (auto it = chunks.begin(); it != chunks.end();)
	{
		if (!isChunkInView(it->first)) //Chunk not in view, unload
		{
			chunkMeshHandler.chunkUnloads.push(it->first);
			it = chunks.erase(it++);
		}
		else
		{
			it++;
		}
	}
	for (int x = centerChunkX - renderDistance; x <= centerChunkX + renderDistance; x++)
	{
		for (int y = centerChunkY - renderDistance; y <= centerChunkY + renderDistance; y++)
		{
			for (int z = centerChunkZ - renderDistance; z <= centerChunkZ + renderDistance; z++)
			{
				if (chunks.find(Vec3i{ x,y,z }) == chunks.end()) //Chunk in view not loaded, load
				{
					updateChunk(Vec3i{ x,y,z });
				}
			}
		}
	}
}

bool ChunkLoader::isChunkInView(Vec3i pos)
{
	if (pos.x <= centerChunkX + renderDistance && pos.x >= centerChunkX - renderDistance
		&& pos.y <= centerChunkY + renderDistance && pos.y >= centerChunkY - renderDistance
		&& pos.z <= centerChunkZ + renderDistance && pos.z >= centerChunkZ - renderDistance)
	{
		return true;
	}
	return false;
}
