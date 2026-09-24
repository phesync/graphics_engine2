#pragma once

constexpr int CHUNK_SIZE = 32;

struct Chunk {
    uint8_t blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    uint32_t render_version = 0;
};