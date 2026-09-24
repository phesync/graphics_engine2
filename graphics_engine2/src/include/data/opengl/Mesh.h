#pragma once

#include <glad/gl.h>

#include <data/MeshData.h>

namespace gpu {
    struct Mesh {
        GLuint vao, vbo, ebo;
        GLsizei index_count;

        static void enable_attributes() {
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

            glEnableVertexAttribArray(3);
            glVertexAttribIPointer(3, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, layer));
        }

        Mesh(MeshData& abstract_mesh) {
            index_count = abstract_mesh.indices.size();

            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, abstract_mesh.vertices.size() * sizeof(Vertex), abstract_mesh.vertices.data(), GL_STATIC_DRAW);

            glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, abstract_mesh.indices.size() * sizeof(unsigned int), abstract_mesh.indices.data(), GL_STATIC_DRAW);

            enable_attributes();
        };

        Mesh(const Vertex vertices[], const int vert_size, const int indices[], const int ind_size) {
            index_count = ind_size;

            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vert_size * sizeof(Vertex), vertices, GL_STATIC_DRAW);

            glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_size * sizeof(unsigned int), indices, GL_STATIC_DRAW);

            enable_attributes();
        }

        ~Mesh() {
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            glDeleteVertexArrays(1, &vao);
        }
    };
}