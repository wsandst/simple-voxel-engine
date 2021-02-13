#include "Renderer.h"


Renderer::Renderer(std::queue<ChunkMesh*> &_chunkMeshes, std::queue<Vec3i> &_chunkUnloads, BlockRegistry &_blockRegistry, TextureManager &_textureManager)
	: newChunkMeshes(_chunkMeshes), chunkUnloads(_chunkUnloads), blockRegistry(_blockRegistry), textureManager(_textureManager)
{

}

Renderer::~Renderer()
{
}

void Renderer::updateDeltatime()
{
	float currentFrame = SDL_GetTicks();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void Renderer::loadNewChunkVBOs()
{
	if (!newChunkMeshes.empty())
	{
		ChunkMesh* newMesh = newChunkMeshes.front();
		if (newMesh->hasStandardBlocks)
		{
			if (chunkStandardVBOs.find(newMesh->chunkPos) == chunkStandardVBOs.end())
			{
				this->vertexCount += newMesh->standardVertices.size();
				chunkStandardVBOs[newMesh->chunkPos] = ChunkBlockVBO(newMesh->chunkPos, newMesh->standardVertices);
			}
			else
			{
				chunkStandardVBOs[newMesh->chunkPos].update(newMesh->standardVertices);
			}
		}
		if (newMesh->hasTransparentBlocks)
		{
			if (chunkTransparentVBOs.find(newMesh->chunkPos) == chunkTransparentVBOs.end())
			{
				this->vertexCount += newMesh->transparentVertices.size();
				chunkTransparentVBOs[newMesh->chunkPos] = ChunkTransparentBlockVBO(newMesh->chunkPos, newMesh->transparentVertices);
			}
			else
			{
				chunkTransparentVBOs[newMesh->chunkPos].update(newMesh->transparentVertices);
			}
		}
		if (newMesh->hasCustomBlocks)
		{
			chunkCustomVBOs[newMesh->chunkPos] = ChunkCustomBlockVBO(newMesh->chunkPos, newMesh->customVertices);
		}
		delete newMesh;
		newChunkMeshes.pop();
	}
}

void Renderer::unloadChunkVBOs()
{
	while (!chunkUnloads.empty())
	{
		this->vertexCount -= chunkStandardVBOs[chunkUnloads.front()].size;
		chunkStandardVBOs.erase(chunkUnloads.front());
		chunkTransparentVBOs.erase(chunkUnloads.front());
		chunkCustomVBOs.erase(chunkUnloads.front());

		chunkUnloads.pop();
	}
}

void Renderer::render()
{
	loadNewChunkVBOs();
	unloadChunkVBOs();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.calculateProjectionMatrix();
	camera.calculateViewMatrix();
	
	glActiveTexture(GL_TEXTURE0);

	drawChunks();
	drawSkybox();
	
	SDL_GL_SwapWindow(window);
}


void Renderer::drawChunks()
{
	drawBlocks();
	drawTransparentBlocks();
	drawCustomBlocks();
}

void Renderer::drawSkybox()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
	glBindVertexArray(0);
	//glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content;
	skyboxShader.use();
	skyboxShader.setMat4("view", camera.getViewMatrix());
	skyboxShader.setMat4("projection", camera.getProjectionMatrix());
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Renderer::drawBlocks()
{
	chunksShader.use();

	glBindTexture(GL_TEXTURE_2D, blockTex);

	for (auto& it : chunkStandardVBOs) { //Draw chunks
		glm::mat4 matrix = glm::mat4(camera.getProjectionMatrix()*camera.getViewMatrix()*it.second.translation);
		chunksShader.setMat4("matrix", matrix);

		glBindVertexArray(it.second.VAO);
		glDrawArrays(GL_TRIANGLES, 0, it.second.size);
	}
}

void Renderer::drawTransparentBlocks()
{
	transparencyShader.use();

	glBindTexture(GL_TEXTURE_2D, blockTex);

	for (auto& it : chunkTransparentVBOs) { //Draw chunks
		glm::mat4 matrix = glm::mat4(camera.getProjectionMatrix()*camera.getViewMatrix()*it.second.translation);
		chunksShader.setMat4("matrix", matrix);

		glBindVertexArray(it.second.VAO);
		glDrawArrays(GL_TRIANGLES, 0, it.second.size);
	}
}

void Renderer::drawCustomBlocks()
{
	customShader.use();

	glBindTexture(GL_TEXTURE_2D, customBlockTex);

	for (auto& it : chunkCustomVBOs) { //Draw chunks
		glm::mat4 matrix = glm::mat4(camera.getProjectionMatrix()*camera.getViewMatrix()*it.second.translation);
		chunksShader.setMat4("matrix", matrix);

		glBindVertexArray(it.second.VAO);
		glDrawArrays(GL_TRIANGLES, 0, it.second.size);
	}
}


void Renderer::init()
{
	camera = Camera(windowWidth, windowHeight);
	generateSkyBoxVertices();

	initSDL();
	initOpenGL();

	initSkyBox();
	initChunks();
}

void Renderer::initOpenGL()
{
	glewExperimental = GL_TRUE;
	glewInit();
	
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(float), &skyboxVertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	skyboxShader = Shader("skybox");
	chunksShader = Shader("chunks");
	customShader = Shader("custom");
	transparencyShader = Shader("transparency");
	glViewport(0, 0, windowWidth, windowHeight);
}

void Renderer::initSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO), 0) {
		std::cout << "SDL could not initialize! SDL_Error:" << SDL_GetError() << std::endl;
	}

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	screenResHeight = DM.h;
	screenResWidth = DM.w;

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetSwapInterval(0);

	//Antialiasing
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, MSAALevel*MSAALevel);
	glEnable(GL_MULTISAMPLE);

	window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	glContext = SDL_GL_CreateContext(window);
}
void Renderer::initChunks()
{
	//stbi_set_flip_vertically_on_load(true);

	glGenTextures(1, &blockTex);
	glBindTexture(GL_TEXTURE_2D, blockTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureManager.blockAtlas.maxTextureSize, textureManager.blockAtlas.maxTextureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureManager.blockAtlas.atlas.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenTextures(1, &customBlockTex);
	glBindTexture(GL_TEXTURE_2D, customBlockTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureManager.customAtlas.maxTextureSize, textureManager.customAtlas.maxTextureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureManager.customAtlas.atlas.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}


void Renderer::initSkyBox()
{
	stbi_set_flip_vertically_on_load(false);
	std::vector <Texture> skyBoxTextures = { FileHandler::loadImage("skybox/front.tga"), FileHandler::loadImage("skybox/back.tga"), FileHandler::loadImage("skybox/up.tga"), FileHandler::loadImage("skybox/down.tga"),
		FileHandler::loadImage("skybox/right.tga"), FileHandler::loadImage("skybox/left.tga") };
	glGenTextures(1, &skyboxTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	for (unsigned int i = 0; i < skyBoxTextures.size(); i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, skyBoxTextures[i].width, skyBoxTextures[i].height, 0, GL_RGB, GL_UNSIGNED_BYTE, skyBoxTextures[i].data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Renderer::close()
{
	SDL_GL_DeleteContext(glContext);
	SDL_Quit();
}

void Renderer::updateResolution()
{

	glViewport(0, 0, windowWidth, windowHeight);

	camera.windowWidth = windowWidth;
	camera.windowHeight = windowHeight;

	render();

}

void Renderer::resizeWindow(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	updateResolution();
}

void Renderer::toggleFullscreen() {

	isFullscreen = !isFullscreen;

	if (isFullscreen)
	{
		minimizedWidth = windowWidth;
		minimizedHeight = windowHeight;
		SDL_SetWindowSize(window, screenResWidth, screenResHeight);
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		windowWidth = screenResWidth;
		windowHeight = screenResHeight;

	}
	else
	{
		SDL_SetWindowFullscreen(window, 0);
		SDL_SetWindowSize(window, minimizedWidth, minimizedHeight);
		windowWidth = minimizedWidth;
		windowHeight = minimizedHeight;
	}

	updateResolution();
}

void Renderer::generateSkyBoxVertices()
{
	const float size = 1000;
	skyboxVertices = std::vector<float>{
		-size,  size, -size,
		-size, -size, -size,
		 size, -size, -size,
		 size, -size, -size,
		 size,  size, -size,
		-size,  size, -size,

		-size, -size,  size,
		-size, -size, -size,
		-size,  size, -size,
		-size,  size, -size,
		-size,  size,  size,
		-size, -size,  size,

		 size, -size, -size,
		 size, -size,  size,
		 size,  size,  size,
		 size,  size,  size,
		 size,  size, -size,
		 size, -size, -size,

		-size, -size,  size,
		-size,  size,  size,
		 size,  size,  size,
		 size,  size,  size,
		 size, -size,  size,
		-size, -size,  size,

		-size,  size, -size,
		 size,  size, -size,
		 size,  size,  size,
		 size,  size,  size,
		-size,  size,  size,
		-size,  size, -size,

		-size, -size, -size,
		-size, -size,  size,
		 size, -size, -size,
		 size, -size, -size,
		-size, -size,  size,
		 size, -size,  size
	};
}
