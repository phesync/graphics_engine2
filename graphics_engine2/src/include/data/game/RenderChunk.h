#pragma once

#include <data/game/BlockInfo.h>
#include <data/game/Chunk.h>

const std::vector<Vertex> top_face = {
    { { -0.5,  0.5, 0.5 }, {0, 1, 0}, {0, 0} },
    { {  0.5,  0.5, 0.5 }, {0, 1, 0}, {1, 0} },
    { {  0.5,  0.5, -0.5 }, {0, 1, 0}, {1, 1} },
    { { -0.5,  0.5, -0.5 }, {0, 1, 0}, {0, 1} }
};

const std::vector<Vertex> bottom_face = {
    { { -0.5,  -0.5, 0.5 }, {0, -1, 0}, {0, 1} },
    { {  0.5,  -0.5, 0.5 }, {0, -1, 0}, {1, 1} },
    { {  0.5,  -0.5, -0.5 }, {0, -1, 0}, {1, 0} },
    { { -0.5,  -0.5, -0.5 }, {0, -1, 0}, {0, 0} }
};

const std::vector<Vertex> left_face = {
    { { -0.5,  -0.5, 0.5 }, {-1, 0, 0}, {1, 0} },
    { { -0.5,  0.5, 0.5 }, {-1, 0, 0}, {1, 1} },
    { { -0.5,  0.5, -0.5 }, {-1, 0, 0}, {0, 1} },
    { { -0.5,  -0.5, -0.5 }, {-1, 0, 0}, {0, 0} }
};

const std::vector<Vertex> right_face = {
    { { 0.5,  -0.5, 0.5 }, {1, 0, 0}, {0, 0} },
    { { 0.5,  0.5, 0.5 }, {1, 0, 0}, {0, 1} },
    { { 0.5,  0.5, -0.5 }, {1, 0, 0}, {1, 1} },
    { { 0.5,  -0.5, -0.5 }, {1, 0, 0}, {1, 0} }
};

const std::vector<Vertex> front_face = {
    { { -0.5, -0.5, 0.5 }, {0, 0, 1}, {0, 0} },
    { {  0.5, -0.5, 0.5 }, {0, 0, 1}, {1, 0} },
    { {  0.5,  0.5, 0.5 }, {0, 0, 1}, {1, 1} },
    { { -0.5,  0.5, 0.5 }, {0, 0, 1}, {0, 1} }
};

const std::vector<Vertex> back_face = {
    { { -0.5, -0.5, -0.5 }, {0, 0, -1}, {1, 0} },
    { {  0.5, -0.5, -0.5 }, {0, 0, -1}, {0, 0} },
    { {  0.5,  0.5, -0.5 }, {0, 0, -1}, {0, 1} },
    { { -0.5,  0.5, -0.5 }, {0, 0, -1}, {1, 1} }
};

class RenderChunk {
private:
    constexpr static int max_vertices = (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) * 24; // ABSOLUTE worst case
    constexpr static int max_indices = max_vertices * 1.5;

    inline static Vertex vert_buffer[max_vertices];
    inline static int ind_buffer[max_indices];

    inline static int vert_index;
    inline static int ind_index;

    void copy_face(const std::vector<Vertex>& face, const glm::vec3 offset, int layer, bool ccw) {
        const int start_vert = vert_index;

        for (const Vertex& vertex : face) {
            vert_buffer[vert_index].pos = vertex.pos + offset;
            vert_buffer[vert_index].normal = vertex.normal;
            vert_buffer[vert_index].uv = vertex.uv;
            vert_buffer[vert_index].layer = layer;

            vert_index++;
        }

        if (ccw) {
            ind_buffer[ind_index++] = start_vert;
            ind_buffer[ind_index++] = start_vert + 1;
            ind_buffer[ind_index++] = start_vert + 2;
            ind_buffer[ind_index++] = start_vert;
            ind_buffer[ind_index++] = start_vert + 2;
            ind_buffer[ind_index++] = start_vert + 3;
        }
        else {
            ind_buffer[ind_index++] = start_vert;
            ind_buffer[ind_index++] = start_vert + 2;
            ind_buffer[ind_index++] = start_vert + 1;
            ind_buffer[ind_index++] = start_vert;
            ind_buffer[ind_index++] = start_vert + 3;
            ind_buffer[ind_index++] = start_vert + 2;
        }
    }

    void reset_buffers() {
        vert_index = 0;
        ind_index = 0;
    }

    gpu::Mesh create_render_chunk(
        const Chunk& chunk,
        const Chunk* chunk_nx,
        const Chunk* chunk_px,
        const Chunk* chunk_ny,
        const Chunk* chunk_py,
        const Chunk* chunk_nz,
        const Chunk* chunk_pz
    ) {
        MeshData data;

        reset_buffers();

        int max = CHUNK_SIZE - 1;

        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    uint8_t block_id = chunk.blocks[x][y][z];

                    const BlockInfo& info = Block_Info[block_id];

                    if (block_id == 0) continue;

                    bool pz = false;
                    bool nz = false;
                    bool px = false;
                    bool nx = false;
                    bool py = false;
                    bool ny = false;

                    if (x != 0) nx = chunk.blocks[x - 1][y][z] == 0;
                    else if (chunk_nx != nullptr) nx = chunk_nx->blocks[max][y][z] == 0;

                    if (x != max) px = chunk.blocks[x + 1][y][z] == 0;
                    else if (chunk_px != nullptr) px = chunk_px->blocks[0][y][z] == 0;

                    if (y != 0) ny = chunk.blocks[x][y - 1][z] == 0;
                    else if (chunk_ny != nullptr) ny = chunk_ny->blocks[x][max][z] == 0;

                    if (y != max) py = chunk.blocks[x][y + 1][z] == 0;
                    else if (chunk_py != nullptr) py = chunk_py->blocks[x][0][z] == 0;

                    if (z != 0) nz = chunk.blocks[x][y][z - 1] == 0;
                    else if (chunk_nz != nullptr) nz = chunk_nz->blocks[x][y][max] == 0;

                    if (z != max) pz = chunk.blocks[x][y][z + 1] == 0;
                    else if (chunk_pz != nullptr) pz = chunk_pz->blocks[x][y][0] == 0;

                    glm::vec3 offset(x, y, z);

                    if (nx) copy_face(left_face, offset, info.textures[2], true);

                    if (px) copy_face(right_face, offset, info.textures[3], false);

                    if (py) copy_face(top_face, offset, info.textures[0], true);

                    if (ny) copy_face(bottom_face, offset, info.textures[1], false);

                    if (nz) copy_face(back_face, offset, info.textures[5], false);

                    if (pz) copy_face(front_face, offset, info.textures[4], true);

                }
            }
        }

        return gpu::Mesh(vert_buffer, vert_index, ind_buffer, ind_index);
    }

public:
    gpu::Mesh render_mesh;
    uint32_t version = 0;

    RenderChunk(
        const Chunk& chunk,
        const uint32_t ver,
        const Chunk* chunk_nx,
        const Chunk* chunk_px,
        const Chunk* chunk_ny,
        const Chunk* chunk_py,
        const Chunk* chunk_nz,
        const Chunk* chunk_pz
    ) :
        render_mesh(create_render_chunk(chunk, chunk_nx, chunk_px, chunk_ny, chunk_py, chunk_nz, chunk_pz)),
        version(ver)
    {
    };
};