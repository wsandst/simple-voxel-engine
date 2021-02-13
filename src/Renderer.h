#pragma once
#include "Shader.h"
#include "Camera.h"
#include "ChunkMesh.h"
#include "Helpers.h"
#include "VBOWrappers.h"
#include "BlockRegistry.h"
#include "TextureManager.h"

#define GLEW_STATIC 
#include "GL/glew.h"

#include "SDL.h"
#include "SDL_opengl.h"

#include "glm/glm.hpp"

#include <vector>
#include <unordered_map>
#include <atomic>
#include <queue>

class Renderer
{
public:
	const char *windowTitle = "Experimental Voxel Engine";
	int windowWidth = 1080, windowHeight = 720;
	bool isFullscreen = false;
	const int MSAALevel = 4;

	float deltaTime = 0.0f;
	int vertexCount = 0;
	Camera camera;

	//Related to chunk loading, connected to ChunkLoader thread
	std::queue<ChunkMesh*> &newChunkMeshes;
	std::queue<Vec3i> &chunkUnloads;

	BlockRegistry& blockRegistry;
	TextureManager& textureManager;

	std::unordered_map<Vec3i, ChunkBlockVBO> chunkStandardVBOs;
	std::unordered_map<Vec3i, ChunkTransparentBlockVBO> chunkTransparentVBOs;
	std::unordered_map<Vec3i, ChunkCustomBlockVBO> chunkCustomVBOs;

	std::vector<float> skyboxVertices = std::vector<float>();

	Renderer(std::queue<ChunkMesh*> &newChunkMeshes, std::queue<Vec3i> &chunkUnloads, BlockRegistry& blockRegistry, TextureManager& textureManager);
	~Renderer();
	void updateDeltatime();
	void loadNewChunkVBOs();
	void unloadChunkVBOs();

	void render();
	void init();
	void close();
	
	void resizeWindow(int width, int height);
	void toggleFullscreen();
private:
	int minimizedWidth = 1280, minimizedHeight = 720;
	int screenResHeight, screenResWidth;


	Shader chunksShader;
	Shader transparencyShader;
	Shader customShader;

	Shader skyboxShader;

	SDL_Window* window;

	SDL_GLContext glContext;

	unsigned int VAO, VBO, skyboxVAO, skyboxVBO, skyboxTex, blockTex, customBlockTex;
	float lastFrame = 0.0f;
	int texArrayCount = 0;

	void initOpenGL();
	void initSDL();
	void initChunks();
	void initSkyBox();
	void generateSkyBoxVertices();

	void drawChunks();
	void drawSkybox();

	void drawBlocks();
	void drawTransparentBlocks();
	void drawCustomBlocks();

	void updateResolution();
};

