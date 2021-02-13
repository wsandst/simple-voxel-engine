#include "BlockType.h"
#include "Chunk.h"
#include "ChunkMesh.h"
#include "ChunkMeshHandler.h"
#include "Helpers.h"

#define GLEW_STATIC 
#include "GL/glew.h"

#include "SDL.h"
#include "SDL_opengl.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <thread>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <atomic>
#include <set>

class ChunkLoader
{
public:

	std::unordered_map<Vec3i, Chunk> chunks;
	std::stack<Vec3i> chunkUpdates;
	std::stack<Vec3i> chunkMeshUpdates;

	bool stopGenerating = false;

	ChunkMeshHandler chunkMeshHandler = ChunkMeshHandler(32);
	const int renderDistance = 10;
	void generateChunk(Vec3i pos, unsigned int fill = 0);
	void meshChunk(Vec3i pos);
	void startChunkLoading();
	void stopChunkLoading();
	void chunkLoadingLoop();
	void updateCenter(int x, int y, int z);
	void updateChunk(Vec3i pos);
	std::queue<ChunkMesh*>& getChunkMeshReference();

	std::queue<Vec3i>& getChunkUnloadReference();

	ChunkLoader();
	~ChunkLoader();
private:
	const int renderDistanceSq = renderDistance * renderDistance;
	std::unordered_set<Vec3i> meshedChunks = std::unordered_set<Vec3i>();

	std::vector<Chunk*> edgeChunks = std::vector<Chunk*>();
	std::vector<Chunk*> inactiveEdgeChunks = std::vector<Chunk*>();
	
	void chunkUpdating(Vec3i pos);
	void linkChunkToAdjacent(Vec3i pos);
	void checkEdgeValidity();

	void unloadChunk(Vec3i pos);
	void updateChunkBoundries();
	bool isChunkInView(Vec3i pos);
	int floorDivideDown(int a, int b);
	int centerChunkX, centerChunkY, centerChunkZ  = 0;
	bool running = false;
	std::atomic<bool> viewUpdated{true};
	std::thread chunkLoadingThread;

	void expandChunkEdges();
	bool isChunkWithinRenderDistance(Chunk* chunk);
	bool isChunkWithinRenderDistance(int x, int y, int z);
};
