#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 textureCoordinates_in;

uniform mat4 uiProj;

layout(location = 0) out vec2 textureCoordinates;

// task 2-1i
void main()
{
	gl_Position = uiProj * vec4(position, 1.0); //vec4(textureCoordinates_in, 0.0, 1.0);//model * vec4(position, 1.0);
    textureCoordinates = textureCoordinates_in;
}