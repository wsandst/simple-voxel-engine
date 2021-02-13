#version 410 core

in vec2 TextureCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
    //FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	FragColor = texture(ourTexture, TextureCoord);
}
