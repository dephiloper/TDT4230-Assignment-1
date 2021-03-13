#pragma once

#include "mesh.h"
#include "utilities/imageLoader.hpp"
#include "utilities/glfont.h"

unsigned int generateBuffer(Mesh &mesh);
unsigned int generateTexture(PNGImage &image);
void indexVBO(
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec2> & in_uvs,
	std::vector<glm::vec3> & in_normals,
	std::vector<glm::vec3> & in_tangents,
	std::vector<glm::vec3> & in_bitangents,

	std::vector<unsigned int> & out_indices,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals,
	std::vector<glm::vec3> & out_tangents,
	std::vector<glm::vec3> & out_bitangents
);
bool getSimilarVertexIndex(
    glm::vec3 &in_vertex,
    glm::vec2 &in_uv,
    glm::vec3 &in_normal,
    std::vector<glm::vec3> &out_vertices,
    std::vector<glm::vec2> &out_uvs,
    std::vector<glm::vec3> &out_normals,
    unsigned int &result);
void computeTangentBasis(
    // inputs
    std::vector<unsigned int> &indices,
    std::vector<glm::vec3> &vertices,
    std::vector<glm::vec2> &uvs,
    std::vector<glm::vec3> &normals,
    // outputs
    std::vector<glm::vec3> &tangents,
    std::vector<glm::vec3> &bitangents);
bool is_near(float v1, float v2);