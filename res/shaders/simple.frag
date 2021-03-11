#version 410 core

struct LightSource {
    vec3 position;
    vec3 color;
};

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec3 vertexPos;

out vec4 color;

uniform vec3 light0;
uniform vec3 light1;
uniform vec3 light2;

uniform LightSource lights[3];

uniform mat4 model;
uniform vec3 viewPos;
uniform vec3 ballPos;

const float ballRadius = 3.0;

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453); }
float dither(vec2 uv) { return (rand(uv)*2.0-1.0) / 256.0; }

vec3 reject(vec3 from, vec3 onto) {
    return from - onto*dot(from, onto)/dot(onto, onto);
}

void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - vertexPos);
    float shininess = 128;
    vec3 vertexBallVec = ballPos - vertexPos;

    float la = pow(10, -2);
    float lb = pow(10, -2);
    float lc = pow(10, -3);

    vec3 outputColor = vec3(0);

    // task 1-1f - ambient light (only one, only white)
    // float ambientStrength = 0.1;
    // vec3 ambient = ambientStrength * vec3(1.0);

    for (int i = 0; i < 3; i++) {

        // task 1-1f - ambient light
        float ambientStrength = 0.1;
        //vec3 ambient = ambientStrength * lights[i].color;
        vec3 ambient = ambientStrength * vec3(1.0);

        // task 1-1g - diffuse light
        vec3 lightVec = lights[i].position - vertexPos;
        vec3 lightDir = normalize(lightVec);
        float diffuseStrength = max(dot(norm, lightDir), 0) * (1 - ambientStrength);
        vec3 diffuse = diffuseStrength * vec3(1.0);//lights[i].color;

        // task 1-1h - specular light
        float specularStrength = 0.1;
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = specularStrength * spec * vec3(1.0);//lights[i].color;

        // task 1-2a - attenuation
        float lightDist = length(lightVec);
        float L = 1 / (1 + la + lightDist * lb + pow(lightDist, 2) * lc);

        // task 1-2c - shadows
        // float lit = 1.0;
        vec3 rej = reject(vertexBallVec, lightVec);
        
        // task 1-2c - hard shadows
        // if (length(rej) < ballRadius)
            // lit = 0.2;

        // task 1-4 - smooth shadows
        float lit = smoothstep(ballRadius - .5, ballRadius + .5, length(rej));
        
        // // Do not cast when:
        // // The length of the vector from fragment to light is shorter than the vector from fragment to ball.
        // // The dot product of thoses two vectors is less than zero.
        if (length(lightVec) < length(vertexBallVec) || dot(vertexBallVec, lightVec) < 0)
            lit = 1.0;

        diffuse *= L * lit;
        specular *= L * lit;

        outputColor += ambient + diffuse + specular;
    }

    // task 1-2b - add dither
    color = vec4(outputColor + dither(textureCoordinates), 1.0);
    //color = vec4(0.5 * normalize(normal) + 0.5, 1.0);
}