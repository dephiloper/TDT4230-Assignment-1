#include "renderer.hpp"

#include "lookup.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

void Renderer::init() {
    // load shaders
    if (gameShader.load("../res/shaders/game.vert", "../res/shaders/game.frag") != 0)
        std::cout << "Error loading shaders" << std::endl;
}

void Renderer::renderMesh(size_t cubeIndex) {
    std::vector<float> verts;
    // int center = 15;

    // for (size_t x = 0; x < 10; x++) {
    //     for (size_t y = 0; y < 10; y++) {
    //         for (size_t z = 0; z < 10; z++) {
    float isolevel = 1;

    // std::vector<float> cube = {};

    // int dist = x + y + z - center;
    // dist = abs(dist);
    // cube.push_back(dist < 3 ? 0 : 1);

    // dist = (x + 1) + y + z - center;
    // dist = abs(dist);
    // cube.push_back(dist < 3 ? 0 : 1);

    // dist = x + y + z - center;
    // dist = abs(dist);
    // cube.push_back((x + 1) + y + (z + 1) - center < 3 ? 0 : 1);
    // cube.push_back(x + y + (z + 1) - center < 3 ? 0 : 1);
    // cube.push_back(x + (y + 1) + z - center < 3 ? 0 : 1);
    // cube.push_back((x + 1) + (y + 1) + z - center < 3 ? 0 : 1);
    // cube.push_back((x + 1) + (y + 1) + (z + 1) - center < 3 ? 0 : 1);
    // cube.push_back(x + (y + 1) + (z + 1) - center < 3 ? 0 : 1);

    // if (cube[0] < isolevel) cubeIndex |= 1;
    // if (cube[1] < isolevel) cubeIndex |= 2;
    // if (cube[2] < isolevel) cubeIndex |= 4;
    // if (cube[3] < isolevel) cubeIndex |= 8;
    // if (cube[4] < isolevel) cubeIndex |= 16;
    // if (cube[5] < isolevel) cubeIndex |= 32;
    // if (cube[6] < isolevel) cubeIndex |= 64;
    // if (cube[7] < isolevel) cubeIndex |= 128;

    std::vector<glm::vec3> vertlist(12);

    /* Find the vertices where the surface intersects the cube */
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

    /* Cube is entirely in/out of the surface */
    if (edgeTable[cubeIndex] != 0) {
        std::vector<glm::vec3> cubeVerts = {
            glm::vec3(-0.5, -0.5, -0.5),  // 0
            glm::vec3(0.5, -0.5, -0.5),  // 1
            glm::vec3(0.5, -0.5, 0.5),  // 2
            glm::vec3(-0.5, -0.5, 0.5),  // 3

            glm::vec3(-0.5, 0.5, -0.5),  // 4
            glm::vec3(0.5, 0.5, -0.5),  // 5
            glm::vec3(0.5, 0.5, 0.5),  // 6
            glm::vec3(-0.5, 0.5, 0.5),  // 7
        };

        // for (size_t i = 0; i < 16; i++)
        // {
        //     std::cout << triTable[cubeIndex][i] << std::endl;
        // }

        if (edgeTable[cubeIndex] & 1)
            vertlist.at(0) = interpolateVerts(cubeVerts[0], cubeVerts[1]);  // edge between 0 - 1
        if (edgeTable[cubeIndex] & 2)
            vertlist.at(1) = interpolateVerts(cubeVerts[1], cubeVerts[2]);  // edge between 1 - 2
        if (edgeTable[cubeIndex] & 4)
            vertlist.at(2) = interpolateVerts(cubeVerts[2], cubeVerts[3]);  // edge between 2 - 3
        if (edgeTable[cubeIndex] & 8)
            vertlist.at(3) = interpolateVerts(cubeVerts[3], cubeVerts[0]);  // edge between 3 - 0
        if (edgeTable[cubeIndex] & 16)
            vertlist.at(4) = interpolateVerts(cubeVerts[4], cubeVerts[5]);  // edge between 4 - 5
        if (edgeTable[cubeIndex] & 32)
            vertlist.at(5) = interpolateVerts(cubeVerts[5], cubeVerts[6]);  // edge between 5 - 6
        if (edgeTable[cubeIndex] & 64)
            vertlist.at(6) = interpolateVerts(cubeVerts[6], cubeVerts[7]);  // edge between 6 - 7
        if (edgeTable[cubeIndex] & 128)
            vertlist.at(7) = interpolateVerts(cubeVerts[7], cubeVerts[4]);  // edge between 7 - 4
        if (edgeTable[cubeIndex] & 256)
            vertlist.at(8) = interpolateVerts(cubeVerts[0], cubeVerts[4]);  // edge between 0 - 4
        if (edgeTable[cubeIndex] & 512)
            vertlist.at(9) = interpolateVerts(cubeVerts[1], cubeVerts[5]);  // edge between 1 - 5
        if (edgeTable[cubeIndex] & 1024)
            vertlist.at(10) = interpolateVerts(cubeVerts[2], cubeVerts[6]);  // edge between 2 - 6
        if (edgeTable[cubeIndex] & 2048)
            vertlist.at(11) = interpolateVerts(cubeVerts[3], cubeVerts[7]);  // edge between 3 - 7
    }

    for (size_t i = 0; triTable[cubeIndex][i] != -1; i += 3) {
        for (size_t v = 0; v < 3; v++) {  // for each vertex
            verts.push_back((vertlist[triTable[cubeIndex][i + v]][0]));
            verts.push_back((vertlist[triTable[cubeIndex][i + v]][1]));
            verts.push_back((vertlist[triTable[cubeIndex][i + v]][2]));
        }
        //for (size_t c = 0; c < 3; c++)  // for each coordinate
        // }
        // }
        // }
    }

    meshVao = loadMesh(verts);
    nTriangles = verts.size();
}

glm::vec3 Renderer::interpolateVerts(glm::vec3 p1, glm::vec3 p2) {
    return (p1 + p2) / 2.0f;
}

void Renderer::render() {
    gameShader.use();

    if ((float)glfwGetTime() > timePassed) {
        renderMesh(timePassed / 2);
        timePassed += 2;
    }

    glm::mat4 model = glm::mat4(1.0f);
    // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0, 0.0, 0.0));
    // model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0, 0.0, 0.0));

    gameShader.setMat4("model", model);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    gameShader.setMat4("view", view);

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    gameShader.setMat4("projection", projection);

    glBindVertexArray(meshVao);
    glDrawArrays(GL_TRIANGLES, 0, nTriangles);
    // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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

unsigned int Renderer::loadMesh(const std::vector<float> &vertices, bool hasColor) {
    unsigned int index = 0;
    unsigned int stride = 3;
    unsigned int offset = 0;

    if (hasColor)
        stride += 3;

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

    // 3. set vertex attribute pointers
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)nullptr);
    glEnableVertexAttribArray(0);
    offset += 3;

    if (hasColor) {  // rgb = 3
        index = 1;
        glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)(offset * sizeof(float)));
        glEnableVertexAttribArray(index);
        offset += 3;
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