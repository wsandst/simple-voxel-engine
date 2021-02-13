#version 410 core

in vec2 TextureCoord;
in float LightLevel;
in float Visibility;
flat in vec2 TileOffset;

out vec4 FragColor;

uniform sampler2D ourTexture;

const float TILE_SIZE = 1.0f / 128.0f;

void main()
{
	vec4 textureColor = texture(ourTexture, vec2(TileOffset.x + mod(TextureCoord.x, TILE_SIZE), TileOffset.y + mod(TextureCoord.y, TILE_SIZE)));
	if (textureColor.w == 0)
	{
		discard;
	}
    FragColor = vec4(textureColor.xyz * LightLevel, 1.0f);
    FragColor = mix(vec4(0.545f, 0.666f, 0.90588f, 1.0), FragColor, Visibility);
}
