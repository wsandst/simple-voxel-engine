#version 410 core

layout (location = 0) in vec3 inPos; //3 bytes
layout (location = 1) in vec2 inQuadDim; //2 bytes
layout (location = 2) in vec2 inTileOffset; //2 bytes
layout (location = 3) in float inLightLevel; //1 byte

uniform mat4 matrix;

out vec2 TextureCoord;
out float LightLevel;
flat out vec2 TileOffset;
out float Visibility;

const float density = 0.0035;
const float gradient = 1.65;

const float TILE_SIZE = 1.0f / 128.0f;

const int MIN_LIGHT_LEVEL = 64;
const int MAX_LIGHT_LEVEL = 256;
const float MAX_LIGHT_DIV = 1.0f / (MIN_LIGHT_LEVEL + MAX_LIGHT_LEVEL);

void main()
{

    gl_Position = matrix * vec4(float(inPos.x), float(inPos.y), float(inPos.z), 1.0f);
	
    TileOffset = vec2(inTileOffset.x * TILE_SIZE, inTileOffset.y * TILE_SIZE);
    TextureCoord = vec2(inQuadDim.x * TILE_SIZE * 0.5f, inQuadDim.y * TILE_SIZE * 0.5f);
	
	LightLevel = (inLightLevel + MIN_LIGHT_LEVEL) * MAX_LIGHT_DIV;
    vec4 positionRelativeToCamera = matrix * gl_Position;

    float distance = length(positionRelativeToCamera.xyz);
    Visibility = exp(-pow((distance*0.0007*density),gradient));
    Visibility = clamp(Visibility,0.0,1.0);
}