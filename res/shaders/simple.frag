#version 410 core

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec3 vertexPos;

out vec4 color;

uniform mat4 model;

uniform vec3 light0;
// uniform vec3 light1;
// uniform vec3 light2;

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453); }
float dither(vec2 uv) { return (rand(uv)*2.0-1.0) / 256.0; }

void main()
{
    //vec3 lights[3] = vec3[3](light0, light1, light2);
    vec3 lightColor = vec3(1.0);

    // 1f - ambient color
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // // 1g - diffuse color
    // for (int i = 0; i < 3; i++) {
    //     // calculate light direction (from vertex to light source)
    //     vec3 lightDir = normalize(lights[i] - vertexPos);

    //     // calculate diffuse intensity (dot of light direction and surface normal)
    //     float diffuseStrength = clamp(dot(lightDir, normal), 0, 1);
    // }

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light0 - vertexPos);
    float diffuseStrength = max(dot(norm, lightDir), 0);
    vec3 diffuse = diffuseStrength * lightColor;

    color = vec4(ambient + diffuse, 1.0);
    //color = vec4(0.5 * normalize(normal) + 0.5 * ambient + diffuse, 1.0);
}