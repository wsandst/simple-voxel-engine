#include "InputHandler.h"

inline int InputHandler::getChunkCoord(int a, int b)
{
	if (a >= 0) return a / b;
	else return (a - b + 1) / b;
}

void InputHandler::updateChunkLoader(ChunkLoader* _chunkLoader)
{
	loader = _chunkLoader;
}

/*
case SDLK_LEFT:
				std::cout << "Left Click Called" << std::endl;
				if (nearbyChunks.size() >= 8) {
					glm::vec3 startPos = camera.getPosition();
					glm::vec3 endPos = startPos + camera.getCameraFront() * camera.cameraStep;
					std::cout << "After cast values: " << "X: " << endPos.x << " Y: " << endPos.y << " Z: " << endPos.z << std::endl;
					for (Chunk chunk : nearbyChunks) {
						if (chunk.x == floor(endPos.x) && chunk.y == floor(endPos.y) && chunk.z == floor(endPos.z)) {
							chunk.setBlock(BlockData(2, 8, 0), abs(endPos.x / 32), abs(endPos.y / 32), abs(endPos.z / 32));
							std::cout << "Place block called" << std::endl;
						}
					}
				}
				break;
*/
// Bad name to avoid redeclaration

void InputHandler::handleInput(Camera &camera, float deltaTime)
{
	camera.cameraStep = 0.5;
	float xOffset, yOffset;
	while (SDL_PollEvent(&sdlEvent) != 0) {
		switch (sdlEvent.type) {
		case SDL_MOUSEBUTTONDOWN:
			switch (sdlEvent.button.button) {
			case SDL_BUTTON_RIGHT:
				placeBlock(camera);
				break;
			case SDL_BUTTON_LEFT:
				breakBlock(camera);
				break;
			}
			break;
		case SDL_KEYDOWN:
			switch (sdlEvent.key.keysym.sym) {
			case SDLK_w:
				camera.moveForward();
				break;
			case SDLK_s:
				camera.moveBack();
				break;
			case SDLK_a:
				camera.moveLeft();
				break;
			case SDLK_d:
				camera.moveRight();
				break;
			case SDLK_SPACE:
				camera.moveUp();
				break;
			case SDLK_LCTRL:
				camera.moveDown();
				break;
			case SDLK_r:
				renderWireframes = !renderWireframes;
				if (renderWireframes)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			case SDLK_l:
				loader->stopGenerating = true;
				break;
			case SDLK_F9:
				std::cout << "x: " << camera.getPosition().x << " y: " << camera.getPosition().y << " z: " << camera.getPosition().z << "\n";
				break;
			case SDLK_t:
				updateTest = true;
				break;
			case SDLK_ESCAPE:
				windowContext = !windowContext;
				if (windowContext) {
					SDL_ShowCursor(0);
					SDL_SetRelativeMouseMode(SDL_TRUE);
				}
				else {
					SDL_ShowCursor(1);
					SDL_SetRelativeMouseMode(SDL_FALSE);
				}
				break;
			case SDLK_F11: //Toggle fullscreen
				std::cout << "Fullscreen toggled" << "\n";
				renderer.toggleFullscreen();
				break;
			break;
			}
		case SDL_MOUSEMOTION:
			int relativeX, relativeY;
			SDL_GetRelativeMouseState(&relativeX, &relativeY);

			camera.updateView(relativeX * sensitivity, -relativeY * sensitivity);
			break;
		case SDL_WINDOWEVENT:
			switch (sdlEvent.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				renderer.resizeWindow(sdlEvent.window.data1, sdlEvent.window.data2);
				printf("Window size changed to %dx%d \n",
					sdlEvent.window.data1,
					sdlEvent.window.data2);
				break;
			case SDL_WINDOWEVENT_CLOSE: //Exiting program
				std::cout << "Exiting program" << "\n";
				quit = true;
				break;
			}
			break;
		}
	}
}

void InputHandler::placeBlock(Camera& camera)
{
	BlockData block = BlockData(3, 0, 0);

	glm::vec3 startPos = glm::vec3(camera.getPosition().x / 2, camera.getPosition().y / 2, camera.getPosition().z / 2);
	glm::vec3 fixedFront = glm::vec3(camera.getCameraFront().x / 2, camera.getCameraFront().y / 2, camera.getCameraFront().z / 2);
	glm::vec3 endPos = startPos + fixedFront * blockCheckStep;

	int stepsTaken = 0;

	int x = endPos.x;
	int y = endPos.y;
	int z = endPos.z;
	int xChunk = getChunkCoord(x, 32);
	int yChunk = getChunkCoord(y, 32);
	int zChunk = getChunkCoord(z, 32);
	int xPrecise = floor(x % 32);
	int yPrecise = floor(y % 32);
	int zPrecise = floor(z % 32);

	if (xPrecise < 0) {
		xPrecise = 31 + xPrecise;
	}
	if (yPrecise < 0) {
		yPrecise = 31 + yPrecise;
	}
	if (zPrecise < 0) {
		zPrecise = 31 + zPrecise;
	}


	int oldXPrecise, oldYPrecise, oldZPrecise;

	if (loader->chunks.find(Vec3i{ xChunk, yChunk, zChunk }) != loader->chunks.end()) {
		while (loader->chunks.at(Vec3i{ xChunk, yChunk, zChunk }).getBlock(xPrecise, yPrecise, zPrecise).getBlockID() == 0) {
			if (stepsTaken > maxBuildingDistance * (1 / blockCheckStep)) { // Make sure its a full unit always unaffected by blockCheckStep
				return;
			}
			endPos = endPos + fixedFront * blockCheckStep;
			x = endPos.x;
			y = endPos.y;
			z = endPos.z;
			xChunk = getChunkCoord(x, 32);
			yChunk = getChunkCoord(y, 32);
			zChunk = getChunkCoord(z, 32);
			oldXPrecise = xPrecise;
			oldYPrecise = yPrecise;
			oldZPrecise = zPrecise;
			xPrecise = x % 32;
			yPrecise = y % 32;
			zPrecise = z % 32;

			if (xPrecise < 0) {
				xPrecise = 31 + xPrecise;
			}
			if (yPrecise < 0) {
				yPrecise = 31 + yPrecise;
			}
			if (zPrecise < 0) {
				zPrecise = 31 + zPrecise;
			}

			//std::cout << "Pos: " << "X: " << x << " Y: " << y << " Z: " << z << std::endl;

			stepsTaken++;
		}

		loader->chunks[Vec3i{ xChunk, yChunk, zChunk }].setBlock(block, oldXPrecise, oldYPrecise, oldZPrecise);

		loader->updateChunk(Vec3i{ xChunk, yChunk, zChunk });
	}
}

void InputHandler::breakBlock(Camera& camera)
{
	BlockData block = BlockData(0, 31, 0);

	glm::vec3 startPos = glm::vec3(camera.getPosition().x / 2, camera.getPosition().y / 2, camera.getPosition().z / 2);
	glm::vec3 fixedFront = glm::vec3(camera.getCameraFront().x / 2, camera.getCameraFront().y / 2, camera.getCameraFront().z / 2);
	glm::vec3 endPos = startPos + fixedFront * blockCheckStep;

	int stepsTaken = 0;

	int x = endPos.x;
	int y = endPos.y;
	int z = endPos.z;
	int xChunk = getChunkCoord(x, 64);
	int yChunk = getChunkCoord(y, 64);
	int zChunk = getChunkCoord(z, 64);
	int xPrecise = x % 32;
	int yPrecise = y % 32;
	int zPrecise = z % 32;
	if (xPrecise < 0) {
		xPrecise = 31 + xPrecise;
	}
	if (yPrecise < 0) {
		yPrecise = 31 + yPrecise;
	}
	if (zPrecise < 0) {
		zPrecise = 31 + zPrecise;
	}

	if (loader->chunks.find(Vec3i{ xChunk, yChunk, zChunk }) != loader->chunks.end()) {
		while (loader->chunks.at(Vec3i{ xChunk, yChunk, zChunk }).getBlock(xPrecise, yPrecise, zPrecise).getBlockID() == 0) {
			if (stepsTaken > maxBuildingDistance * (1 / blockCheckStep)) { // Make sure its a full unit always unaffected by blockCheckStep
				return;
			}
			endPos = endPos + fixedFront * blockCheckStep;
			x = endPos.x;
			y = endPos.y;
			z = endPos.z;
			xChunk = getChunkCoord(x, 64);
			yChunk = getChunkCoord(y, 64);
			zChunk = getChunkCoord(z, 64);
			xPrecise = x % 32;
			yPrecise = y % 32;
			zPrecise = z % 32;
			if (xPrecise < 0) {
				xPrecise = 31 + xPrecise;
			}
			if (yPrecise < 0) {
				yPrecise = 31 + yPrecise;
			}
			if (zPrecise < 0) {
				zPrecise = 31 + zPrecise;
			}
			//std::cout << "Pos: " << "X: " << x << " Y: " << y << " Z: " << z << std::endl;

			stepsTaken++;
		}

		loader->chunks[Vec3i{ xChunk, yChunk, zChunk }].setBlock(block, xPrecise, yPrecise, zPrecise);
		loader->updateChunk(Vec3i{ xChunk, yChunk, zChunk });
	}
}

InputHandler::InputHandler(Renderer& _renderer) : renderer(_renderer)
{
	SDL_SetRelativeMouseMode(SDL_TRUE);
}


InputHandler::~InputHandler()
{
}
