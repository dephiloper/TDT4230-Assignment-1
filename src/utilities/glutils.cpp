#include <glad/glad.h>
#include <program.hpp>
#include "glutils.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include <iostream>

template <class T>
unsigned int generateAttribute(int id, int elementsPerEntry, std::vector<T> data, bool normalize)
{
    unsigned int bufferID;
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(id, elementsPerEntry, GL_FLOAT, normalize ? GL_TRUE : GL_FALSE, sizeof(T), 0);
    glEnableVertexAttribArray(id);
    return bufferID;
}

unsigned int generateBuffer(Mesh &mesh)
{
    unsigned int vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    generateAttribute(0, 3, mesh.vertices, false);
    generateAttribute(1, 3, mesh.normals, true);
    if (mesh.textureCoordinates.size() > 0)
    {
        generateAttribute(2, 2, mesh.textureCoordinates, false);
    }

    if (mesh.tangents.size() > 0) {
        generateAttribute(3, 3, mesh.tangents, false);
    }

    if (mesh.bitangents.size() > 0) {
        generateAttribute(4, 3, mesh.bitangents, false);
    }

    unsigned int indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

    return vaoID;
}

// task 2-1c&d
unsigned int generateTexture(PNGImage &image)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());

    // task 2-1d
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

// task 2-3g
void computeTangentBasis(
    // inputs
    std::vector<unsigned int> &indices,
    std::vector<glm::vec3> &vertices,
    std::vector<glm::vec2> &uvs,
    std::vector<glm::vec3> &normals,
    // outputs
    std::vector<glm::vec3> &tangents,
    std::vector<glm::vec3> &bitangents)
{
    std::vector<unsigned int> counts(vertices.size(), 0);
    tangents.resize(vertices.size(), glm::vec3(0));
    bitangents.resize(vertices.size(), glm::vec3(0));

    for (int i = 0; i < indices.size(); i += 3)
    {
        unsigned int index1 = indices[i]; 
        unsigned int index2 = indices[i+1]; 
        unsigned int index3 = indices[i+2]; 

        // Shortcuts for vertices
        glm::vec3 &v0 = vertices[index1];
        glm::vec3 &v1 = vertices[index2];
        glm::vec3 &v2 = vertices[index3];

        // Shortcuts for UVs
        glm::vec2 &uv0 = uvs[index1];
        glm::vec2 &uv1 = uvs[index2];
        glm::vec2 &uv2 = uvs[index3];

        // Edges of the triangle : position delta
        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;

        // UV delta
        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;
        
        tangents.at(index1) += tangent;
        tangents.at(index2) += tangent;
        tangents.at(index3) += tangent;
        bitangents.at(index1) += bitangent;
        bitangents.at(index2) += bitangent;
        bitangents.at(index3) += bitangent;
        counts.at(index1)++;
        counts.at(index2)++;
        counts.at(index3)++;
    }

    // average the tangents and bitangents
    for (size_t i = 0; i < counts.size(); i++)
    {
        tangents.at(i) /= counts.at(i);
        bitangents.at(i) /= counts.at(i);
    }
    
}