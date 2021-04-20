#include "renderer.hpp"

#include "lookup.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/noise.hpp>

void Renderer::init() {
    // load shaders
    if (gameShader.load("../res/shaders/game.vert", "../res/shaders/game.frag") != 0)
        std::cout << "Error loading shaders" << std::endl;
}

void Renderer::renderSphere(size_t gridSize) {
    std::vector<float> verts;
    std::vector<float> normals;

    //unsigned int gridSize = 50;
    glm::vec3 center(gridSize / 2.0f);

    for (size_t x = 0; x < gridSize; x++) {
        for (size_t y = 0; y < gridSize; y++) {
            for (size_t z = 0; z < gridSize; z++) {
                int cubeIndex = 0;  // represents the configuration of a cube (e.g. 0, 1, 2 are below the iso level 00000111 -> cubeIndex = 7)
                float isolevel = center.x;

                // set the cube coordinates of a individual grid cube
                /* 4--------5
                  / |     / |
                7---|----6  |
                |  0-----|--1
                | /      | /
                3--------2
                */
                std::vector<glm::vec3> cubeCoords = {
                    glm::vec3(x, y, z),          // 0
                    glm::vec3(x + 1, y, z),      // 1
                    glm::vec3(x + 1, y, z + 1),  // 2
                    glm::vec3(x, y, z + 1),      // 3

                    glm::vec3(x, y + 1, z),          // 4
                    glm::vec3(x + 1, y + 1, z),      // 5
                    glm::vec3(x + 1, y + 1, z + 1),  // 6
                    glm::vec3(x, y + 1, z + 1),      // 7
                };

                std::vector<float> cube;

                // calculate the distance to the center and put it as
                // values for the current cube
                for (size_t i = 0; i < cubeCoords.size(); i++) {
                    float dist = glm::length(cubeCoords[i] - center);
                    //std::cout << "dist: " << dist << std::endl;
                    cube.push_back(dist);
                }

                // 256 possible cube/mesh types (= 2^8 <- cube vertices)
                if (cube[0] < isolevel) cubeIndex |= 1;
                if (cube[1] < isolevel) cubeIndex |= 2;
                if (cube[2] < isolevel) cubeIndex |= 4;
                if (cube[3] < isolevel) cubeIndex |= 8;
                if (cube[4] < isolevel) cubeIndex |= 16;
                if (cube[5] < isolevel) cubeIndex |= 32;
                if (cube[6] < isolevel) cubeIndex |= 64;
                if (cube[7] < isolevel) cubeIndex |= 128;

                std::vector<glm::vec3> edgeVerts(12);

                if (edgeTable[cubeIndex] != 0) {  //if the Cube is not entirely in/out of the surface

                    /* 4--------5
                      / |     / |
                    7---|----6  |
                    |  0-----|--1
                    | /      | /
                    3--------2
                    0: (-0.5, -0.5, -0.5)
                    1: ( 0.5, -0.5, -0.5)
                    2: ( 0.5, -0.5,  0.5)
                    3: (-0.5, -0.5,  0.5)
                    4: (-0.5,  0.5, -0.5)
                    5: ( 0.5,  0.5, -0.5)
                    6: ( 0.5,  0.5,  0.5)
                    7: (-0.5,  0.5,  0.5)
                    */
                    std::vector<glm::vec3> cubeVerts = {
                        glm::vec3(0, 0, 0),  // 0
                        glm::vec3(1, 0, 0),  // 1
                        glm::vec3(1, 0, 1),  // 2
                        glm::vec3(0, 0, 1),  // 3

                        glm::vec3(0, 1, 0),  // 4
                        glm::vec3(1, 1, 0),  // 5
                        glm::vec3(1, 1, 1),  // 6
                        glm::vec3(0, 1, 1),  // 7
                    };

                    // Find the vertices where the surface intersects the cube
                    // based on the previously calculated cubeIndex
                    if (edgeTable[cubeIndex] & 1)
                        edgeVerts.at(0) = interpolateVerts(cubeVerts[0], cubeVerts[1]);  // edge between 0 - 1
                    if (edgeTable[cubeIndex] & 2)
                        edgeVerts.at(1) = interpolateVerts(cubeVerts[1], cubeVerts[2]);  // edge between 1 - 2
                    if (edgeTable[cubeIndex] & 4)
                        edgeVerts.at(2) = interpolateVerts(cubeVerts[2], cubeVerts[3]);  // edge between 2 - 3
                    if (edgeTable[cubeIndex] & 8)
                        edgeVerts.at(3) = interpolateVerts(cubeVerts[3], cubeVerts[0]);  // edge between 3 - 0
                    if (edgeTable[cubeIndex] & 16)
                        edgeVerts.at(4) = interpolateVerts(cubeVerts[4], cubeVerts[5]);  // edge between 4 - 5
                    if (edgeTable[cubeIndex] & 32)
                        edgeVerts.at(5) = interpolateVerts(cubeVerts[5], cubeVerts[6]);  // edge between 5 - 6
                    if (edgeTable[cubeIndex] & 64)
                        edgeVerts.at(6) = interpolateVerts(cubeVerts[6], cubeVerts[7]);  // edge between 6 - 7
                    if (edgeTable[cubeIndex] & 128)
                        edgeVerts.at(7) = interpolateVerts(cubeVerts[7], cubeVerts[4]);  // edge between 7 - 4
                    if (edgeTable[cubeIndex] & 256)
                        edgeVerts.at(8) = interpolateVerts(cubeVerts[0], cubeVerts[4]);  // edge between 0 - 4
                    if (edgeTable[cubeIndex] & 512)
                        edgeVerts.at(9) = interpolateVerts(cubeVerts[1], cubeVerts[5]);  // edge between 1 - 5
                    if (edgeTable[cubeIndex] & 1024)
                        edgeVerts.at(10) = interpolateVerts(cubeVerts[2], cubeVerts[6]);  // edge between 2 - 6
                    if (edgeTable[cubeIndex] & 2048)
                        edgeVerts.at(11) = interpolateVerts(cubeVerts[3], cubeVerts[7]);  // edge between 3 - 7
                }

                // loop over all the triangles from the triangle table at the specific
                // triTable[cubeIndex] gives a list of triangles for the current cube config (max. 16 triangles in one cube)
                // triTable[cubeIndex][i + v] gives one of the subsequent 3 triangles in the list
                // 1. edgeVerts[triTable[cubeIndex][i + v]][n] gives the x, y, z coord of the edge for the current triangle
                // 2. we transform with x, y, z to position the triangles in the correct voxel of the grid
                // 3. we scale the vertex, to stay inside 0 - 1 on all 3 axis

                for (size_t i = 0; triTable[cubeIndex][i] != -1; i += 3) {
                    for (size_t v = 0; v < 3; v++) {                              // for each vertex
                        float xCoord = edgeVerts[triTable[cubeIndex][i + v]][0];  // 1.
                        xCoord += x;                                              // 2.
                        xCoord /= (float)gridSize;                                // 3.
                        xCoord -= 0.5f;                                           // 2.

                        float yCoord = edgeVerts[triTable[cubeIndex][i + v]][1];  // 1.
                        yCoord += y;                                              // 2.
                        yCoord /= (float)gridSize;                                // 3.
                        yCoord -= 0.5f;                                           // 2.

                        float zCoord = edgeVerts[triTable[cubeIndex][i + v]][2];  // 1.
                        zCoord += z;                                              // 2.
                        zCoord /= (float)gridSize;                                // 3.
                        zCoord -= 0.5f;                                           // 2.

                        verts.push_back(xCoord);
                        verts.push_back(yCoord);
                        verts.push_back(zCoord);
                    }

                    size_t size = verts.size();
                    glm::vec3 a = glm::vec3(verts[size - 9], verts[size - 8], verts[size - 7]);
                    glm::vec3 b = glm::vec3(verts[size - 6], verts[size - 5], verts[size - 4]);
                    glm::vec3 c = glm::vec3(verts[size - 3], verts[size - 2], verts[size - 1]);

                    glm::vec3 norm = glm::normalize(glm::cross(b - a, c - a));

                    for (size_t j = 0; j < 3; j++) {
                        normals.push_back(norm.x);
                        normals.push_back(norm.y);
                        normals.push_back(norm.z);
                    }
                }
            }
        }
    }

    meshVao = loadMesh(verts, normals);
    nTriangles = verts.size();
}

glm::vec3 Renderer::interpolateVerts(glm::vec3 p1, glm::vec3 p2) {
    return (p1 + p2) / 2.0f;
}

void Renderer::render() {
    gameShader.use();

    glm::mat4 model = glm::mat4(1.0f);
    // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, (float)glfwGetTime() * 0.2f, glm::vec3(0.0, 1.0, 0.0));
    model = glm::scale(model, glm::vec3(1.5, 1.5, 1.5));

    gameShader.setMat4("model", model);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    gameShader.setMat4("view", view);

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    gameShader.setMat4("projection", projection);

    std::cout << glm::sin(glfwGetTime() * 0.2) * 0.5f + 0.5f << std::endl;
    int gridSize = (int) ((glm::sin(glfwGetTime() * 0.5f) * 0.5f + 0.5f) * 25.0f);
    std::cout << gridSize << std::endl;
    renderSphere(gridSize);

    glBindVertexArray(meshVao);
    glDrawArrays(GL_TRIANGLES, 0, nTriangles);
}

unsigned int Renderer::loadObject(const std::vector<float> &vertices, const std::vector<unsigned int> &indices, unsigned short dimensions, bool hasColor, bool hasTexture) {
    unsigned int index = 0;
    unsigned int stride = dimensions;
    unsigned int offset = 0;

    if (hasColor)
        stride += 3;
    if (hasTexture)
        stride += 2;

    unsigned int VBO, VAO, EBO;  // create unique id
    glGenVertexArrays(1, &VAO);  // create vertex array object
    glGenBuffers(1, &VBO);       // create buffer object to corresponding id
    glGenBuffers(1, &EBO);       // create buffer object to corresponding id

    // 1. bind buffers and vertex array object
    // -------------------------------------------------------------------
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  // bind created buffer to buffer type GL_ARRAY_BUFFER (type of vertex buffer)

    // after binding, every buffer calls on GL_ARRAY_BUFFER will be used to configure VBO

    // 2. copy vertices array in a buffer for opengl to use
    // copies vertex data into buffer memory
    // 0: type of buffer we want copy data into
    // 1: size of the data
    // 2: the data
    // 3: specifies how the graphics card should manage the given data
    // 3.1: GL_STATIC_DRAW (data does not change or very rarely)
    // 3.2: GL_DYNAMIC_DRAW (data changes a lot)
    // 3.3: GL_STREAM_DRAW (data> changes on every draw) -> 3.2 & 3.3 data is placed in memory for faster drawing

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 3. set vertex attribute pointers
    glVertexAttribPointer(index, dimensions, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)nullptr);
    glEnableVertexAttribArray(0);
    offset += dimensions;

    if (hasColor) {  // rgb = 3
        index = 1;
        glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)(offset * sizeof(float)));
        glEnableVertexAttribArray(index);
        offset += 3;
    }

    if (hasTexture) {  // uv = 2
        index = 2;
        glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)(offset * sizeof(float)));
        glEnableVertexAttribArray(index);
    }

    // unbind buffer and vertex array so VAO calls don't accidentally modify VAO, but it's not necessary
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

unsigned int Renderer::loadMesh(const std::vector<float> &vertices, const std::vector<float> &normals, bool hasColor) {
    unsigned int index = 0;
    unsigned int stride = 3;
    unsigned int offset = 0;

    if (hasColor)
        stride += 3;

    unsigned int VBO[2];
    unsigned int VAO;            // create unique id
    glGenVertexArrays(1, &VAO);  // create vertex array object
    glGenBuffers(2, VBO);        // create buffers object to corresponding id

    // 1. bind buffers and vertex array object
    // -------------------------------------------------------------------
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);  // bind created buffer to buffer type GL_ARRAY_BUFFER (type of vertex buffer)

    // after binding, every buffer calls on GL_ARRAY_BUFFER will be used to configure VBO

    // 2. copy vertices array in a buffer for opengl to use
    // copies vertex data into buffer memory
    // 0: type of buffer we want copy data into
    // 1: size of the data
    // 2: the data
    // 3: specifies how the graphics card should manage the given data
    // 3.1: GL_STATIC_DRAW (data does not change or very rarely)
    // 3.2: GL_DYNAMIC_DRAW (data changes a lot)
    // 3.3: GL_STREAM_DRAW (data> changes on every draw) -> 3.2 & 3.3 data is placed in memory for faster drawing

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // 3. set vertex attribute pointers
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)nullptr);
    glEnableVertexAttribArray(index);
    index++;
    offset += 3;

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)nullptr);
    glEnableVertexAttribArray(index);
    index++;
    offset += 3;

    if (hasColor) {  // rgb = 3
        glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)(offset * sizeof(float)));
        glEnableVertexAttribArray(index);
    }

    // unbind buffer and vertex array so VAO calls don't accidentally modify VAO, but it's not necessary
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

// unsigned int
// Renderer::loadTexture(unsigned int vao, const std::string &texturePath, bool alphaChannel)
// {
//     GLenum format = alphaChannel ? GL_RGBA : GL_RGB;

//     glBindVertexArray(vao);

//     unsigned int texture;
//     glGenTextures(1, &texture);
//     glBindTexture(GL_TEXTURE_2D, texture);

//     int width, height, nrChannels;
//     unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
//     if (data)
//     {
//         // glTexImage2D parameters
//         /* The first argument specifies the texture target; setting this to GL_TEXTURE_2D means this operation will generate a texture on the currently bound texture object at the same target (so any textures bound to targets GL_TEXTURE_1D or GL_TEXTURE_3D will not be affected).
//          * The second argument specifies the mipmap level for which we want to create a texture for if you want to set each mipmap level manually, but we'll leave it at the base level which is 0.
//          * The third argument tells OpenGL in what kind of format we want to store the texture. Our image has only RGB values so we'll store the texture with RGB values as well.
//          * The 4th and 5th argument sets the width and height of the resulting texture. We stored those earlier when loading the image so we'll use the corresponding variables.
//          * The next argument should always be 0 (some legacy stuff).
//          * The 7th and 8th argument specify the format and datatype of the source image. We loaded the image with RGB values and stored them as chars (bytes) so we'll pass in the corresponding values.
//          * The last argument is the actual image data.
//          */
//         glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//     }
//     else
//     {
//         std::cout << "Failed to load texture - path: " << texturePath << std::endl;
//     }

//     // set the texture wrapping/filtering options (on the currently bound texture object)
//     glGenerateMipmap(GL_TEXTURE_2D);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

//     // free image resources
//     stbi_image_free(data);

//     // unbind buffer and vertex array so VAO calls don't accidentally modify VAO, but it's not necessary
//     glBindTexture(GL_TEXTURE_2D, 0);
//     glBindVertexArray(0);

//     return texture;
// }