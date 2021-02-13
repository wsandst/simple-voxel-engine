#include <iostream>
#include <vector>
#include <atomic>

#include "Renderer.h"
#include "ChunkMeshHandler.h"
#include "InputHandler.h"
#include "TextureManager.h"


bool quit;

int main(int argc, char** argv) {
	ChunkLoader chunkLoader;
	TextureManager textureManager;

	BlockRegistry blockRegistry = BlockRegistry();
	blockRegistry.loadBlocks(textureManager);
	blockRegistry.blockTypes;

	textureManager.generateBlockAtlas(blockRegistry.blockTypes);
	textureManager.generateCustomAtlas(blockRegistry.blockTypes);

	chunkLoader.chunkMeshHandler.blockTypes = blockRegistry.blockTypes;

	Renderer renderer(chunkLoader.getChunkMeshReference(), chunkLoader.getChunkUnloadReference(), blockRegistry, textureManager);
	InputHandler inputHandler(renderer);

	chunkLoader.updateCenter(renderer.camera.getPosition().x, renderer.camera.getPosition().y, renderer.camera.getPosition().z);
	chunkLoader.startChunkLoading();
	inputHandler.updateChunkLoader(&chunkLoader);
	renderer.init();

	quit = false;
	int frameCount = 0;

	while (!quit) {
		if (inputHandler.windowContext) {
			renderer.render();
			if (frameCount % 180 == 0)
			{
				std::cout << "Frame took " << renderer.deltaTime << " ms (" << 1000 / renderer.deltaTime << " fps) @ " << (renderer.vertexCount / 6) << " vertices (" << renderer.chunkStandardVBOs.size() << "VBOs) \n";
			}
			frameCount++;
		}
		inputHandler.handleInput(renderer.camera, renderer.deltaTime);

		if (inputHandler.quit)
		{
			quit = true;
			chunkLoader.stopChunkLoading();
		}

		chunkLoader.updateCenter(renderer.camera.getPosition().x, renderer.camera.getPosition().y, renderer.camera.getPosition().z);
		renderer.updateDeltatime();
		frameCount++;
	}
	renderer.close();

	return 0;
}