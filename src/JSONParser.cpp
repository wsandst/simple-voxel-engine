#include "JSONParser.h"



void JSONParser::parseBlocks(std::vector<BlockType>& blockTypes, std::unordered_map<std::string, int>& blockIDmap, TextureManager& textureManager)
{
	json blocksJSON;
	std::ifstream file("assets/blocks.json");
	file >> blocksJSON;

	int blockIDcounter = 1;
	bool loadedBlockIDfile = loadBlockIDmap(blockIDmap, blockIDcounter);

	for (size_t i = 0; i < blockIDcounter; i++)
	{
		BlockType nullBlock;
		nullBlock.isTransparent = true;
		blockTypes.push_back(nullBlock);
	}

	int blockID;
	bool isTransparent;

	std::vector<BlockModelType> blockModels = std::vector<BlockModelType>();

	BlockModelType modelType;
	for (json::iterator it = blocksJSON.begin(); it != blocksJSON.end(); ++it) {
		std::string blockName = it.key();
		json block = it.value();
		blockModels.clear();
		blockModels.push_back(BLOCK);

		if (block.find("model") != block.end()) //Custom block model, add separetly
		{
			std::string textureName = block.at("texture");
			std::string modelName = block.at("model");
			blockID = registerBlockID(blockIDmap, blockName, blockIDcounter);
			textureManager.insertTextureToCustomMap(textureName, blockID);
			BlockType blockType = BlockType(blockName, blockID);
			blockType.blockModelType = CUSTOM;
			blockType.customModel = FileHandler::loadObj(modelName);

			blockIDmap[blockType.name] = blockID;
			blockID++;
			blockTypes.push_back(blockType);
			continue;
		}

		if (block.find("transparent") != block.end())
		{
			isTransparent = block.at("transparent");
		}
		else
		{
			isTransparent = false;
		}


		if (block.find("slabs") != block.end())
		{
			blockModels.insert(blockModels.end(), { SLAB_BOTTOM, SLAB_TOP, SLAB_RIGHT, SLAB_LEFT, SLAB_BACK, SLAB_FRONT });
		}
		if (block.find("slopes") != block.end())
		{
			blockModels.insert(blockModels.end(), { SLOPE_UP_FRONT, SLOPE_UP_BACK, SLOPE_UP_RIGHT, SLOPE_UP_LEFT, SLOPE_DOWN_FRONT, SLOPE_DOWN_BACK, SLOPE_DOWN_RIGHT, SLOPE_DOWN_LEFT });
		}

		for (size_t i = 0; i < blockModels.size(); i++)
		{
			blockID = registerBlockID(blockIDmap, blockName, blockIDcounter);
			BlockType blockType = BlockType(blockName, blockID);
			blockType.blockModelType = blockModels[i];
			blockType.name += getBlockModelName(blockType.blockModelType);
			blockType.isTransparent = isTransparent;
			blockType.isNonSolid = (blockModels[i] != BLOCK || isTransparent);
			if (block.find("texture") != block.end()) //Texture loading
			{
				//Foun
				if (block.at("texture").is_string()) //Default, same texture for all faces
				{
					std::string name = block.at("texture");
					textureManager.insertTextureToMap(name, blockID, BACK);
					textureManager.insertTextureToMap(name, blockID, FRONT);
					textureManager.insertTextureToMap(name, blockID, TOP);
					textureManager.insertTextureToMap(name, blockID, BOTTOM);
					textureManager.insertTextureToMap(name, blockID, RIGHT);
					textureManager.insertTextureToMap(name, blockID, LEFT);
				}
				else if (block.at("texture").is_object()) //Different faces
				{
					json texturedata = block.at("texture");
					std::cout << texturedata << std::endl;
					std::string defaultTexture;
					if (texturedata.find("default") != texturedata.end())
					{
						std::string name = texturedata.at("default");
						defaultTexture = name;
					}
					if (texturedata.find("front") != texturedata.end())
					{
						textureManager.insertTextureToMap(texturedata.at("front"), blockID, FRONT);
					}
					else
					{
						textureManager.insertTextureToMap(defaultTexture, blockID, FRONT);
					}
					if (texturedata.find("back") != texturedata.end())
					{
						textureManager.insertTextureToMap(texturedata.at("back"), blockID, BACK);
					}
					else
					{
						textureManager.insertTextureToMap(defaultTexture, blockID, BACK);
					}
					if (texturedata.find("top") != texturedata.end())
					{
						textureManager.insertTextureToMap(texturedata.at("top"), blockID, TOP);
					}
					else
					{
						textureManager.insertTextureToMap(defaultTexture, blockID, TOP);
					}
					if (texturedata.find("bottom") != texturedata.end())
					{
						textureManager.insertTextureToMap(texturedata.at("bottom"), blockID, BOTTOM);
					}
					else
					{
						textureManager.insertTextureToMap(defaultTexture, blockID, BOTTOM);
					}
					if (texturedata.find("left") != texturedata.end())
					{
						textureManager.insertTextureToMap(texturedata.at("left"), blockID, LEFT);
					}
					else
					{
						textureManager.insertTextureToMap(defaultTexture, blockID, LEFT);
					}
					if (texturedata.find("right") != texturedata.end())
					{
						textureManager.insertTextureToMap(texturedata.at("right"), blockID, RIGHT);
					}
					else
					{
						textureManager.insertTextureToMap(defaultTexture, blockID, RIGHT);
					}
				}
			}

			if (blockTypes.size() > blockID)
			{
				blockTypes[blockID] = blockType;
			}
			else
			{
				blockTypes.push_back(blockType);
			}
		}
	}

	file.close();

	saveBlockIDmap(blockIDmap);
	//Go through the blocks and load the relevant information into BlockTypes
	//BlockTypes need to contain the vertices
}

bool JSONParser::loadBlockIDmap(std::unordered_map<std::string, int>& blockIDmap, int &highestID)
{
	std::string path = "resources/internal/block_id_map.dat";
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "No block id map was loaded. Generating blockID map from scratch..." << std::endl;
		return false;
	}
	std::string line;
	std::vector<std::string> splitString;
	int id;
	while (getline(file, line)) {
		// using printf() in all tests for consistency
		splitString = FileHandler::split(line, ':');
		id = stoi(splitString[1]);
		highestID = std::max(highestID, id);
		blockIDmap[splitString[0]] = id;
	}
	id++;
	highestID = id;
	std::cout << "Loaded stored block id map successfully" << std::endl;
	return true;
}

void JSONParser::saveBlockIDmap(std::unordered_map<std::string, int>& blockIDmap)
{
	std::string output = "";
	for (auto& it : blockIDmap)
	{
		output += it.first + ": " + std::to_string(it.second) + "\n";
	}
	FileHandler::saveFile("assets/internal/block_id_map.dat", output);
	std::cout << "Saved block id map successfully" << std::endl;
}

std::string JSONParser::getBlockModelName(BlockModelType type)
{
	std::map<BlockModelType, std::string> blockModelNameMap = {
		{BLOCK,            ""                },
		{SLAB_TOP,         "slab_top"        },
		{SLAB_BOTTOM,      "slab_bottom"     },
		{SLAB_RIGHT,       "slab_right"      },
		{SLAB_LEFT,        "slab_left"       },
		{SLAB_FRONT,       "slab_front"      },
		{SLAB_BACK,        "slab_back"       },
		{SLOPE_UP_FRONT,   "slope_up_front"  },
		{SLOPE_UP_BACK,    "slope_up_back"   },
		{SLOPE_UP_RIGHT,   "slope_up_right"  },
		{SLOPE_UP_LEFT,    "slope_up_left"   },
		{SLOPE_DOWN_FRONT, "slope_down_front"},
		{SLOPE_DOWN_BACK,  "slope_down_back" },
		{SLOPE_DOWN_RIGHT, "slope_down_right"},
		{SLOPE_DOWN_LEFT,  "slope_down_left" }
	};
	return blockModelNameMap[type];
}

int JSONParser::registerBlockID(std::unordered_map<std::string, int>& blockIDmap, std::string& blockName, int& id)
{
	if (blockIDmap.find(blockName) != blockIDmap.end()) //Block already registered
	{
		return blockIDmap[blockName];
	}
	int blockID = id;
	blockIDmap[blockName] = blockID;
	id++;
	return blockID;
}

//Static class, never used
JSONParser::JSONParser()
{
}


JSONParser::~JSONParser()
{
}