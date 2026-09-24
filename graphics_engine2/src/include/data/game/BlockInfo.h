#pragma once

#include <vector>
#include <string>

struct BlockInfo {
    uint8_t textures[6]; // top, bottom, left, right, front, back
};

const BlockInfo Block_Info[] = {
    {}, // air
    {1, 2, 0, 0, 0, 0}, // 1: grass
    {2, 2, 2, 2, 2, 2}, // 2: dirt
};

const std::vector<std::string> Block_Texture = {
    "assets/textures/blocks/grass_side.png", // 0
    "assets/textures/blocks/grass_top.png", // 1
    "assets/textures/blocks/grass_bottom.png", // 2
};