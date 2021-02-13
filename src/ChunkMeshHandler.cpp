#include "ChunkMeshHandler.h"

ChunkMeshHandler::~ChunkMeshHandler()
{

}

void ChunkMeshHandler::addChunk(Chunk& chunk)
{
	std::vector<StandardVertexAttrib> standardVertices = std::vector<StandardVertexAttrib>();
	std::vector<StandardVertexAttrib> transparentVertices = std::vector<StandardVertexAttrib>();
	std::vector<CustomVertexAttrib> customVertices = std::vector<CustomVertexAttrib>();
	//std::cout << "Size of FaceData: " << sizeof(FaceData) << std::endl;
	generateGreedyMesh(standardVertices, transparentVertices, chunk);
	//generateChunkMesh(standardVertices, transparentVertices, customVertices, chunk);
	Vec3i chunkPos = { chunk.x, chunk.y, chunk.z };
	ChunkMesh *chunkMesh = new ChunkMesh(chunkPos, standardVertices, transparentVertices, customVertices);
	newChunkMeshes.push(chunkMesh);
}

void ChunkMeshHandler::removeChunk(Chunk& chunk)
{
	chunkUnloads.push(Vec3i{ chunk.x, chunk.y, chunk.z });
}

void ChunkMeshHandler::clearGreedyMask()
{
	std::fill(greedyMask, greedyMask + 32*32, FaceData());
}

inline ChunkMeshHandler::FaceData ChunkMeshHandler::getFaceData(const Chunk& chunk, const int& x, const int& y, const int& z, const BlockFace& side)
{
	BlockData block = chunk.blocks[z * 32 * 32 + y * 32 + x];
	int id = block.getBlockID();
	bool transparent = blockTypes[id].isTransparent;
	if (id == 0) //Air
	{
		return FaceData();
	}
	unsigned char vl1, vl2, vl3, vl4;
	bool visible = true; //Output var
	getVertexLight(chunk, transparent, x, y, z, side, vl1, vl2, vl3, vl4, visible);
	if (!visible)
		return FaceData();
	FaceData data = FaceData(blockTypes[id].textures.getSide(side), transparent, vl1, vl2, vl3, vl4);
	return data;
}

void ChunkMeshHandler::createQuad(std::vector<StandardVertexAttrib>& mesh, const Vec3i& bottomLeft, const Vec3i& topLeft, const Vec3i& topRight, const Vec3i& bottomRight, int width, int height, const FaceData& face, const BlockFace& side)
{
	const int tW = 2;
	width *= tW;
	height *= tW;
	switch (side)
	{
	case NEGY: case NEGZ: case POSY: case POSZ:
	{
		const StandardVertexAttrib vertices[6] = {
			StandardVertexAttrib(topLeft.x * tW, topLeft.y * tW, topLeft.z * tW,
				width, height, face.texture.x, face.texture.y, face.light[0]),
			StandardVertexAttrib(bottomLeft.x * tW, bottomLeft.y * tW, bottomLeft.z * tW,
				0, height, face.texture.x, face.texture.y, face.light[2]),
			StandardVertexAttrib(bottomRight.x * tW, bottomRight.y * tW, bottomRight.z * tW,
				0, 0, face.texture.x, face.texture.y, face.light[3]),
			StandardVertexAttrib(bottomRight.x * tW, bottomRight.y * tW, bottomRight.z * tW,
				0, 0, face.texture.x,  face.texture.y, face.light[3]),
			StandardVertexAttrib(topRight.x * tW, topRight.y * tW, topRight.z * tW,
				width, 0, face.texture.x, face.texture.y, face.light[1]),
			StandardVertexAttrib(topLeft.x * tW, topLeft.y * tW, topLeft.z * tW,
				width, height,  face.texture.x, face.texture.y, face.light[0])
		};
		mesh.insert(mesh.end(), &vertices[0], &vertices[6]);
		break;
	}
	case NEGX: case POSX:
	{
		const StandardVertexAttrib vertices[6] = {
			StandardVertexAttrib(topLeft.x * tW, topLeft.y * tW, topLeft.z * tW,
				0, 0, face.texture.x, face.texture.y, face.light[0]),
			StandardVertexAttrib(topRight.x * tW, topRight.y * tW, topRight.z * tW,
				height, 0, face.texture.x, face.texture.y, face.light[1]),
			StandardVertexAttrib(bottomRight.x * tW, bottomRight.y * tW, bottomRight.z * tW,
				height, width, face.texture.x, face.texture.y, face.light[3]),
			StandardVertexAttrib(bottomRight.x * tW, bottomRight.y * tW, bottomRight.z * tW,
				height, width, face.texture.x,  face.texture.y, face.light[3]),
			StandardVertexAttrib(bottomLeft.x * tW,  bottomLeft.y * tW,  bottomLeft.z * tW,
				0, width, face.texture.x, face.texture.y, face.light[2]),
			StandardVertexAttrib(topLeft.x * tW, topLeft.y * tW, topLeft.z * tW,
				0, 0,  face.texture.x, face.texture.y, face.light[0])
		};
		mesh.insert(mesh.end(), &vertices[0], &vertices[6]);
		break;
	}
	}
}

void ChunkMeshHandler::getVertexLight(const Chunk& chunk, const bool transparent, const int& x, const int& y, const int& z, const BlockFace& side, 
	unsigned char &vl1, unsigned char& vl2, unsigned char& vl3, unsigned char& vl4, bool& visible)
{
	const BlockData frontData = chunk.getBlock(x + (side == POSX) - (side == NEGX),
		y + (side == POSY) - (side == NEGY),
		z + (side == POSZ) - (side == NEGZ));
	int frontID = frontData.getBlockID();
	bool isAir = (frontData.getBlockID() == 0);
	if (!isAir && (!blockTypes[frontID].isTransparent || blockTypes[frontID].isTransparent && transparent)) //Cull block, not visible
	{
		visible = false;
		return;
	}
	if (SMOOTH_LIGHTING)
	{
		int offsetY[4];
		int offsetX[4];
		int offsetZ[4];
		switch (side)
		{
		case POSX:
			offsetX[0] = 0, offsetX[1] = 0, offsetX[2] = 0, offsetX[3] = 0;
			offsetY[0] = 0, offsetY[1] = 0, offsetY[2] = -1, offsetY[3] = -1;
			offsetZ[0] = -1, offsetZ[1] = 0, offsetZ[2] = -1, offsetZ[3] = 0;
			break;
		case NEGX:
			offsetX[0] = -1, offsetX[1] = -1, offsetX[2] = -1, offsetX[3] = -1;
			offsetY[0] = 0, offsetY[1] = 0, offsetY[2] = -1, offsetY[3] = -1;
			offsetZ[0] = -1, offsetZ[1] = 0, offsetZ[2] = -1, offsetZ[3] = 0;
			break;
		case POSZ:
			offsetX[0] = 0, offsetX[1] = 0, offsetX[2] = -1, offsetX[3] = -1;
			offsetY[0] = -1, offsetY[1] = 0, offsetY[2] = -1, offsetY[3] = 0;
			offsetZ[0] = 0, offsetZ[1] = 0, offsetZ[2] = 0, offsetZ[3] = 0;
			break;
		case NEGZ:
			offsetX[0] = 0, offsetX[1] = 0, offsetX[2] = -1, offsetX[3] = -1;
			offsetY[0] = -1, offsetY[1] = 0, offsetY[2] = -1, offsetY[3] = 0;
			offsetZ[0] = -1, offsetZ[1] = -1, offsetZ[2] = -1, offsetZ[3] = -1;
			break;
		case POSY:
			offsetX[0] = 0, offsetX[1] = 0, offsetX[2] = -1, offsetX[3] = -1;
			offsetY[0] = 0, offsetY[1] = 0, offsetY[2] = 0, offsetY[3] = 0;
			offsetZ[0] = -1, offsetZ[1] = 0, offsetZ[2] = -1, offsetZ[3] = 0;
			break;
		case NEGY:
			offsetX[0] = 0, offsetX[1] = 0, offsetX[2] = -1, offsetX[3] = -1;
			offsetY[0] = -1, offsetY[1] = -1, offsetY[2] = -1, offsetY[3] = -1;
			offsetZ[0] = -1, offsetZ[1] = 0, offsetZ[2] = -1, offsetZ[3] = 0;
			break;
		default:
			offsetX[0] = 0, offsetX[1] = 0, offsetX[2] = 0, offsetX[3] = 0;
			offsetY[0] = 0, offsetY[1] = 0, offsetY[2] = 0, offsetY[3] = 0;
			offsetZ[0] = 0, offsetZ[1] = 0, offsetZ[2] = 0, offsetZ[3] = 0;
		}
		for (size_t i = 0; i < 4; i++)
		{
			short light_level = 0;
			for (size_t z_step = 0; z_step < 2; z_step++)
			{
				for (size_t y_step = 0; y_step < 2; y_step++)
				{
					for (size_t x_step = 0; x_step < 2; x_step++)
					{
						light_level += chunk.getBlock(x + offsetX[i] + x_step, y + offsetY[i] + y_step, z + offsetZ[i] + z_step).getLightLevel();
					}
				}
			}
			switch (i) {
			case 0:
				vl1 = light_level;
				break;
			case 1:
				vl2 = light_level;
				break;
			case 2:
				vl3 = light_level;
				break;
			case 3:
				vl4 = light_level;
				break;
			}
		}
	}
	else //Flat
	{
		const int lightLevel = (!isAir ? frontData.getLightLevel() : 31);
		vl1 = lightLevel;
		vl2 = lightLevel;
		vl3 = lightLevel;
		vl4 = lightLevel;
	}
}


void ChunkMeshHandler::generateGreedyMesh(std::vector<StandardVertexAttrib>& standardVertices, std::vector<StandardVertexAttrib>& transparentVertices, const Chunk& chunk)
{
	//standardVertices.reserve(1000);
	
	//_mm_prefetch((char*)chunk.blocks, 0);
	
	
	//Go through 3 'slices': x,y,z
	//Check both sides per slice (so 6 sides in total)

	//Variables for looping and keeping track of the slices
	int i, j, k, l, w, h, u, v, n;
	BlockFace side = LEFT;

	int x[3] = {0, 0, 0};
	int q[3] = {0, 0, 0};
	int du[3] = {0, 0, 0};
	int dv[3] = {0, 0, 0};

	//The two stored faces used for comparison
	FaceData face1, face2;

	const FaceData null = FaceData();
	bool backFace = true, b = false;
	//Go through twice, once for backface = true, another for backface = false
	for (bool backFace = true, b = false; b != backFace; backFace = backFace && b, b = !b) {
		for (int d = 0; d < 3; d++) {
			//Go through the three slices in the backface bool direction
			switch (d) {
			case 0:
				u = 1;
				v = 2;
				break;
			case 1:
				u = 0;
				v = 2;
				break;
			case 2:
				u = 0;
				v = 1;
			}

			x[0] = 0;
			x[1] = 0;
			x[2] = 0;

			q[0] = 0;
			q[1] = 0;
			q[2] = 0;
			q[d] = 1;

			if (d == 0) { side = backFace ? FRONT : BACK; }
			else if (d == 1) { side = backFace ? BOTTOM : TOP; }
			else if (d == 2) { side = backFace ? LEFT : RIGHT; }

			for (x[d] = -1; x[d] < 32;) {
				n = 0;

				//Generate greedy mask
				for (x[v] = 0; x[v] < 32; x[v]++) {
					for (x[u] = 0; x[u] < 32; x[u]++) {

						//Retrieve two voxel faces for comparison
						face1 = (x[d] >= 0) ? getFaceData(chunk, x[0], x[1], x[2], side) : null;
						face2 = (x[d] < 32 - 1) ? getFaceData(chunk, x[0] + q[0], x[1] + q[1], x[2] + q[2], side) : null;

						//This is a mess, but seems to work
						greedyMask[n++] = (((!face1.isNull) && (!face2.isNull) && (face1 == face2)) ? null : (backFace ? face2 : face1));
					}
				}

				x[d]++;

				//Generate mesh from mask
				n = 0;

				for (j = 0; j < 32; j++) {

					for (i = 0; i < 32;) {

						if (!greedyMask[n].isNull) {

							//Compute mesh heigth
							for (w = 1; (i + w < 32) && !greedyMask[n + w].isNull && greedyMask[n + w] == greedyMask[n]; w++) {}

							//Compute mesh width
							bool done = false;

							for (h = 1; j + h < 32; h++) {

								for (k = 0; k < w; k++) {

									if (greedyMask[n + k + h * 32].isNull || !(greedyMask[n + k + h * 32] == greedyMask[n])) { done = true; break; }
								}

								if (done) { break; }
							}

							//Check for visibility to make sure to not mesh any occluded surfaces
							if (!greedyMask[n].isNull) {
								//Add quad
								x[u] = i;
								x[v] = j;

								du[0] = 0;
								du[1] = 0;
								du[2] = 0;
								du[u] = w;

								dv[0] = 0;
								dv[1] = 0;
								dv[2] = 0;
								dv[v] = h;

								//Add quad with the face properties of greedy mask
								if (greedyMask[n].transparent)
								{
									createQuad(transparentVertices,
										Vec3i{ x[0], x[1], x[2] },
										Vec3i{ x[0] + du[0], x[1] + du[1], x[2] + du[2] },
										Vec3i{ x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2] },
										Vec3i{ x[0] + dv[0], x[1] + dv[1], x[2] + dv[2] },
										w,
										h,
										greedyMask[n],
										side);
								}
								else
								{
									createQuad(standardVertices, 
										Vec3i { x[0], x[1], x[2] },
										Vec3i { x[0] + du[0], x[1] + du[1], x[2] + du[2] },
										Vec3i{ x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2] },
										Vec3i { x[0] + dv[0], x[1] + dv[1], x[2] + dv[2] },
										w,
										h,
										greedyMask[n],
										side);
								}
							}

							//Reset the mask
							for (l = 0; l < h; ++l) {

								for (k = 0; k < w; ++k) { greedyMask[n + k + l * 32] = null; }
							}

							//Increment counters and continue
							i += w;
							n += w;

						}
						else {

							i++;
							n++;
						}
					}
				}

			}
		}
	}
}

inline bool ChunkMeshHandler::isBlockAir(int index, Chunk& chunk)
{
	return (chunk.blocks[index].getBlockID() == 0 || blockTypes[chunk.blocks[index].getBlockID()].isNonSolid);
}

void ChunkMeshHandler::registerBlockType(BlockType newType)
{
	blockTypes[newType.blockID] = newType;
}

ChunkMeshHandler::ChunkMeshHandler(int maxBlockTypes)
{
	blockTypes = std::vector<BlockType>(maxBlockTypes*maxBlockTypes);
	textureOffset = float(1) / maxBlockTypes;
}

ChunkMeshHandler::ChunkMeshHandler()
{
}