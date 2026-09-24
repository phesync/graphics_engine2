#pragma once

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <util/stb_image.h>

struct GraphicsImage {
    int width;
    int height;
    int channels;

    unsigned char* data;

    GraphicsImage(const std::string& path) {
        stbi_set_flip_vertically_on_load(true);

        data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);
    };

    ~GraphicsImage() {
        stbi_image_free(data);
    };
};