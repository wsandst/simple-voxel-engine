#pragma once
#include "ChunkLoader.h"
#include "Camera.h"
#include "Renderer.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include <vector>

class InputHandler
{
private:
	Renderer& renderer;

	SDL_Event sdlEvent;
	float sensitivity = 0.5;
	ChunkLoader* loader;
	inline int getChunkCoord(int a, int b);
	float blockCheckStep = 0.5f;
	int maxBuildingDistance = 40;
public:
	void updateChunkLoader(ChunkLoader* _chunkLoader);
	bool windowContext = true;
	bool renderWireframes = false;
	bool updateTest = false;
	bool quit = false;
	void handleInput(Camera &camera, float deltaTime);
	void placeBlock(Camera& camera);
	void breakBlock(Camera& camera);
	InputHandler(Renderer& _renderer);
	~InputHandler();
};

