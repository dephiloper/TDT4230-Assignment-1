#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
// #include <stb_image.h>
#include <filesystem>

#include "map"
#include "vector"
#include "filesystem"
#include "algorithm"

#include "shader.h"

class Renderer
{
public:
    int screenWidth = 800;
    int screenHeight = 600;
    std::string shaderDir;
    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};
    unsigned int triangleVao;

    void init();
    void render();

private:
    Shader gameShader{};
    static unsigned int loadObject(const std::vector<float> &vertices, unsigned short dimensions, bool hasColor = false, bool hasTexture = false);
    // static unsigned int loadTexture(unsigned int vao, const std::string& texturePath, bool alphaChannel);
};

#endif