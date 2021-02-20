#version 410 core

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec3 vertexPos;

out vec4 color;

uniform mat4 model;
uniform vec3 light0;
uniform vec3 light1;
uniform vec3 light2;
uniform vec3 viewPos;
uniform vec3 ballPos;

const float ballRadius = 3.0;

// uniform vec3 light1;
// uniform vec3 light2;

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453); }
float dither(vec2 uv) { return (rand(uv)*2.0-1.0) / 256.0; }

vec3 reject(vec3 from, vec3 onto) {
    return from - onto*dot(from, onto)/dot(onto, onto);
}

void main()
{
    vec3 lights[3] = vec3[3](light0, light1, light2);
    vec3 lightColor = vec3(1.0);

    // 1f - ambient light
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
    vec3 viewDir = normalize(viewPos - vertexPos);
    float shininess = 32;
    vec3 vertexBallVec = ballPos - vertexPos;

    float la = pow(10, -3);
    float lb = pow(10, -3);
    float lc = pow(10, -3);

    vec3 outputColor = vec3(0);

    for (int i = 0; i < 3; i++) {
        // 1g - diffuse light
        vec3 lightVec = lights[i] - vertexPos;
        vec3 lightDir = normalize(lightVec);
        float diffuseStrength = max(dot(norm, lightDir), 0) * (1 - ambientStrength);
        vec3 diffuse = diffuseStrength * lightColor;

        // 1h - specular light
        float specularStrength = 0.5;
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = specularStrength * spec * lightColor;

        // 2a - attenuation
        float lightDist = length(lightVec);
        float L = 1 / (1 + la + lightDist * lb + pow(lightDist, 2) * lc);

        // 2c - shadows
        float lit = 1.0;
        vec3 rej = reject(vertexBallVec, lightVec);
        if (length(rej) < ballRadius)
            lit = 0.2;

        // Do not cast when:
        // The length of the vector from fragment to light is shorter than the vector from fragment to ball.
        // The dot product of thoses two vectors is less than zero.
        if (length(lightVec) < length(vertexBallVec) || dot(vertexBallVec, lightVec) < 0)
            lit = 1.0;

        diffuse *= L * lit;
        specular *= L * lit;

        outputColor += diffuse + specular;
    }

    // 2b - add dither
    color = vec4(ambient + outputColor + dither(textureCoordinates), 1.0);
    //color = vec4(0.5 * normalize(normal) + 0.5 * ambient + diffuse, 1.0);
}