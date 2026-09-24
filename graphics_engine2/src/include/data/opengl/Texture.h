#pragma once

#include <glad/gl.h>

#include <data/GraphicsImage.h>

namespace gpu {
	struct Texture {
        GLuint handle;

		Texture(const GraphicsImage& image) {
            glGenTextures(1, &handle);
            glBindTexture(GL_TEXTURE_2D, handle);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
		}

        ~Texture() {
            glDeleteTextures(1, &handle);
        }
	};
}