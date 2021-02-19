#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 textureCoordinates_in;

uniform mat4 MVP;

layout(location = 0) out vec3 normal;
layout(location = 1) out vec2 textureCoordinates;

void main()
{
    normal = normal_in;
    textureCoordinates = textureCoordinates_in;
    gl_Position = MVP * vec4(position, 1.0f);
}
