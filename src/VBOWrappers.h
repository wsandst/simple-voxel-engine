#pragma once
#include "Chunk.h"
#include "ChunkMesh.h"
#include "Helpers.h"

#define GLEW_STATIC 
#include "GL/glew.h"

#include "SDL.h"
#include "SDL_opengl.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>

class ChunkBlockVBO
{
public:
	unsigned int VBO;
	unsigned int VAO;
	glm::mat4 translation;
	int size;

	ChunkBlockVBO() {};
	ChunkBlockVBO(Vec3i pos, std::vector<StandardVertexAttrib> &vertices)
	{
		create(vertices);
		setPosition(pos);
	};
	~ChunkBlockVBO()
	{
	};
	void create(std::vector<StandardVertexAttrib> &vertices)
	{
		size = vertices.size();
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (vertices.size() > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StandardVertexAttrib), &vertices[0], GL_STATIC_DRAW);
		}
		//Positions
		glVertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(StandardVertexAttrib), nullptr);
		glEnableVertexAttribArray(0);

		//Quad dim									
		glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(StandardVertexAttrib), (void*)offsetof(StandardVertexAttrib, quadDim));
		glEnableVertexAttribArray(1);		

		//Texture offsets
		glVertexAttribPointer(2, 2, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(StandardVertexAttrib), (void*)offsetof(StandardVertexAttrib, textureOffsets));
		glEnableVertexAttribArray(2);

		//Light level
		glVertexAttribPointer(3, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(StandardVertexAttrib), (void*)offsetof(StandardVertexAttrib, lightLevel));
		glEnableVertexAttribArray(3);
	}
	void update(std::vector<StandardVertexAttrib> &vertices)
	{
		size = vertices.size();
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (vertices.size() > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StandardVertexAttrib), &vertices[0], GL_STATIC_DRAW);
		}
	}
	void remove()
	{
		
	}
	void setPosition(Vec3i pos, int chunkSize = 32)
	{
		translation = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * chunkSize * 2, pos.y * chunkSize * 2, pos.z * chunkSize * 2));
	}
};

class ChunkCustomBlockVBO
{
public:
	unsigned int VBO;
	unsigned int VAO;
	glm::mat4 translation;
	int size;

	ChunkCustomBlockVBO() {};
	ChunkCustomBlockVBO(Vec3i pos, std::vector<CustomVertexAttrib> &vertices)
	{
		create(vertices);
		setPosition(pos);
	};
	~ChunkCustomBlockVBO()
	{
	};
	void create(std::vector<CustomVertexAttrib> &vertices)
	{
		size = vertices.size();
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (vertices.size() > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CustomVertexAttrib), &vertices[0], GL_STATIC_DRAW);
		}
		//Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CustomVertexAttrib), nullptr);
		glEnableVertexAttribArray(0);

		//Texture coords								
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CustomVertexAttrib), (void*)offsetof(CustomVertexAttrib, uv));
		glEnableVertexAttribArray(1);

	}
	void update(std::vector<CustomVertexAttrib> &vertices)
	{
		size = vertices.size();
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (vertices.size() > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STREAM_DRAW);
		}
	}
	void remove()
	{

	}
	void setPosition(Vec3i pos, int chunkSize = 32)
	{
		translation = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * 32, pos.y * 32, pos.z * 32));
	}
};

class ChunkTransparentBlockVBO
{
public:
	unsigned int VBO;
	unsigned int VAO;
	glm::mat4 translation;
	int size;

	ChunkTransparentBlockVBO() {};
	ChunkTransparentBlockVBO(Vec3i pos, std::vector<StandardVertexAttrib> &vertices)
	{
		create(vertices);
		setPosition(pos);
	};
	~ChunkTransparentBlockVBO()
	{
	};
	void create(std::vector<StandardVertexAttrib> &vertices)
	{
		size = vertices.size();
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (vertices.size() > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StandardVertexAttrib), &vertices[0], GL_STATIC_DRAW);
		}
		//Positions
		glVertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(StandardVertexAttrib), nullptr);
		glEnableVertexAttribArray(0);

		//Quad dim									
		glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(StandardVertexAttrib), (void*)offsetof(StandardVertexAttrib, quadDim));
		glEnableVertexAttribArray(1);

		//Texture offsets
		glVertexAttribPointer(2, 2, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(StandardVertexAttrib), (void*)offsetof(StandardVertexAttrib, textureOffsets));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(StandardVertexAttrib), (void*)offsetof(StandardVertexAttrib, lightLevel));
		glEnableVertexAttribArray(3);
	}
	void update(std::vector<StandardVertexAttrib> &vertices)
	{
		size = vertices.size();
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (vertices.size() > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StandardVertexAttrib), &vertices[0], GL_STATIC_DRAW);
		}
	}
	void remove()
	{

	}
	void setPosition(Vec3i pos, int chunkSize = 32)
	{
		translation = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * chunkSize * 2, pos.y * chunkSize * 2, pos.z * chunkSize * 2));
	}
};

class ChunkLiquidVBO
{
	unsigned int VBO;
	unsigned int VAO;
	glm::mat4 translation;
	int size;

	ChunkLiquidVBO() {};
	ChunkLiquidVBO(Vec3i pos, std::vector<StandardVertexAttrib> &vertices)
	{
		create(vertices);
		setPosition(pos);
	};
	~ChunkLiquidVBO()
	{
	};
	void create(std::vector<StandardVertexAttrib> &vertices)
	{
		size = vertices.size();
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (vertices.size() > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StandardVertexAttrib), &vertices[0], GL_STATIC_DRAW);
		}
		//Positions
		glVertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(StandardVertexAttrib), nullptr);
		glEnableVertexAttribArray(0);

		//Quad dim									
		glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(StandardVertexAttrib), (void*)offsetof(StandardVertexAttrib, quadDim));
		glEnableVertexAttribArray(1);

		//Texture offsets
		glVertexAttribPointer(2, 2, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(StandardVertexAttrib), (void*)offsetof(StandardVertexAttrib, textureOffsets));
		glEnableVertexAttribArray(2);
	}
	void update(std::vector<StandardVertexAttrib> &vertices)
	{
		size = vertices.size();
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (vertices.size() > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STREAM_DRAW);
		}
	}
	void remove()
	{

	}
	void setPosition(Vec3i pos, int chunkSize = 32)
	{
		translation = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * 32, pos.y * 32, pos.z * 32));
	}
};
