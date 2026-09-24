#pragma once

#include <glad/gl.h>

namespace gpu {
    enum class ShaderType {
        VERTEX,
        FRAGMENT
    };

    int get_gl_shader(ShaderType shader_type) {
        switch (shader_type) {
        case ShaderType::VERTEX: return GL_VERTEX_SHADER;
        case ShaderType::FRAGMENT: return GL_FRAGMENT_SHADER;
        }
    }

	struct Shader {
		GLuint handle;

		Shader(const char* text, ShaderType shader_type) {
            GLint success;
            GLchar info_log[512];

            handle = glCreateShader(get_gl_shader(shader_type));

            glShaderSource(handle, 1, &text, NULL);
            glCompileShader(handle);

            glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

            if (!success) {
                glGetShaderInfoLog(handle, 512, NULL, info_log);

                std::cout << "Shader failed to compile: " << info_log << "\n";
            }
		}

        ~Shader() {
            glDeleteShader(handle);
        }
	};
}