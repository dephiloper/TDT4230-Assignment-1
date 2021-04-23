#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <GLFW/glfw3.h>
#include <glad/glad.h>
// #include <stb_image.h>
#include <filesystem>

#include "algorithm"
#include "filesystem"
#include "map"
#include "shader.hpp"
#include "vector"

class Renderer {
   public:
    int screenWidth = 800;
    int screenHeight = 600;
    std::string shaderDir;

    std::vector<float> vertices = {
        0.0, 0.5, 0.0,    // top
        -0.5, -0.5, 0.5,  // bottom left
        0.5, -0.5, 0.5,   // bottom right
        0.5, -0.5, -0.5,  // top right
        -0.5, -0.5, -0.5  // top left
    };
    std::vector<unsigned int> indices = {
        // note that we start from 0!
        1, 2, 0,
        2, 3, 0,
        3, 4, 0,
        4, 1, 0,
        1, 2, 3,
        1, 3, 4};
    unsigned int meshVao;
    unsigned int nTriangles;

    void init();
    void render();

   private:
    Shader gameShader{};
    unsigned int loadObject(const std::vector<float> &vertices, const std::vector<unsigned int> &indices, unsigned short dimensions, bool hasColor = false, bool hasTexture = false);
    unsigned int loadMesh(const std::vector<float> &vertices, const std::vector<float> &normals, bool hasColor = false);
    glm::vec3 interpolateVerts(glm::vec3 p1, glm::vec3 p2, float isolevel, float v1, float v2);
    void computeMesh(glm::vec4 seed, size_t gridSize, float density);
};

#endif  //SHADER_HPP