#pragma once

#include <vector>
#include <glm/vec3.hpp>

#include <data/Vertex.h>

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    MeshData(const std::vector<Vertex>& c_vertices, const std::vector<unsigned int>& c_indices) :
        vertices(c_vertices),
        indices(c_indices)
    {};

    MeshData() :
        vertices({}),
        indices({})
    {}
};