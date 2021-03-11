#version 410 core

layout(location = 0) in vec2 textureCoordinates;
uniform sampler2D textureSampler;
out vec4 color;

// task 2-1i&j
void main()
{
	color = texture(textureSampler, textureCoordinates);
}