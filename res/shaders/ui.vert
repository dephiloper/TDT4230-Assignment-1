#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 textureCoordinates_in;

uniform mat4 uiProj;
uniform mat4 model;

layout(location = 0) out vec2 textureCoordinates;

// task 2-1i
void main()
{
	gl_Position = uiProj * model * vec4(position, 1.0);
    textureCoordinates = textureCoordinates_in;
}