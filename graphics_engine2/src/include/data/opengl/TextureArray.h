#pragma once

#include <glad/gl.h>

#include <data/GraphicsImage.h>

namespace gpu {
    struct TextureArray {
        GLuint handle;

        void upload(const GraphicsImage& image, int layer) {
            glBindTexture(GL_TEXTURE_2D_ARRAY, handle);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, image.width, image.height, 1, GL_RGB, GL_UNSIGNED_BYTE, image.data);

            //glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        }

        TextureArray(int width, int height, int layers) {
            glGenTextures(1, &handle);
            glBindTexture(GL_TEXTURE_2D_ARRAY, handle);

            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, width, height, layers, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        }

        ~TextureArray() {
            glDeleteTextures(1, &handle);
        }
    };
}