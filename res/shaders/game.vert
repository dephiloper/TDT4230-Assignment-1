#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal_in;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 colorNormal;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    colorNormal = normal_in;
    normal = normalize((model * vec4(normal_in, 0.0)).xyz);
}