#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 textureCoordinates_in;

uniform mat4 MVP;
uniform mat4 model;

uniform mat3 normal_mat;

layout(location = 0) out vec3 normal;
layout(location = 1) out vec2 textureCoordinates;
layout(location = 2) out vec3 vertexPos;

void main()
{
    // task 1e - transform normal by normal matrix
    normal = normalize(normal_mat * normal_in);

    // vertex position in world space (multiplied by model matrix)
    vertexPos = vec3(model * vec4(position, 1.0));
    
    textureCoordinates = textureCoordinates_in;
    gl_Position = MVP * vec4(position, 1.0f);
}
