#pragma once

#include <glad/gl.h>

#include <data/opengl/Shader.h>

namespace gpu {
	struct ShaderUniform {
		GLuint handle;

		void set(const glm::vec3& value) const {
			glUniform3fv(handle, 1, glm::value_ptr(value));
		};

		void set(const float value) const {
			glUniform1fv(handle, 1, &value);
		};

		void set(const glm::mat4& value) const {
			glUniformMatrix4fv(handle, 1, false, glm::value_ptr(value));
		};

		ShaderUniform(GLuint h) : 
			handle(h) 
		{};
	};

	struct ShaderProgram {
		GLuint handle;
		
		ShaderUniform get_uniform(const std::string& name) const {
			GLuint loc = glGetUniformLocation(handle, name.c_str());

			return ShaderUniform(loc);
		}

		ShaderProgram(const std::vector<Shader*>& shaders) {
			handle = glCreateProgram();

			for (const Shader* shader : shaders) {
				glAttachShader(handle, shader->handle);
			}

			glLinkProgram(handle);
		};

		~ShaderProgram() {
			glDeleteProgram(handle);
		}
	};
}