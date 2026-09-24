#pragma once

#include <glad/gl.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <data/opengl/Mesh.h>
#include <data/opengl/Texture.h>
#include <data/opengl/Shader.h>
#include <data/opengl/ShaderProgram.h>
#include <data/opengl/TextureArray.h>
#include <util/ReadText.h>
#include <engine/Resources.h>

struct CameraUniforms {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 view_rotation;
};

template <typename T>
struct UniformBuffer {
    GLuint handle;
    T uniforms;
};

enum class RendererProgram {
    DEFAULT,
    SKYBOX
};

class GLRenderer {
private:
    UniformBuffer<CameraUniforms> camera_ubo;

    int active_mesh_indices = -1;

public:
    void set_viewport(double w, double h) {
        glViewport(0, 0, w, h);
    }

    void set_view(const glm::mat4x4& projection, const glm::mat4x4& view, const glm::mat4x4& view_rot) {
        camera_ubo.uniforms.projection = projection;
        camera_ubo.uniforms.view = view;
        camera_ubo.uniforms.view_rotation = view_rot;

        glBindBuffer(GL_UNIFORM_BUFFER, camera_ubo.handle);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUniforms), &camera_ubo.uniforms);
    }

    void set_wireframe(bool wireframe) {
        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    //void set_light_dir(const glm::vec3& light_dir) {
        //GLint light_loc = glGetUniformLocation(gpu_resources::shaders::default_shader->handle, "light_dir");
        //glUniform3fv(light_loc, 1, glm::value_ptr(light_dir));
    //}

    void clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void use_texture(gpu::Texture* texture) {
        if (texture == nullptr) return;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->handle);
    }

    void use_texture_array(gpu::TextureArray* texture_array) {
        if (texture_array == nullptr) return;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array->handle);
    }

    void use_program(gpu::ShaderProgram* program) {
        if (program == nullptr) return;
            
        glUseProgram(program->handle);
    }

    void use_mesh(const gpu::Mesh* mesh) {
        if (mesh != nullptr) {
            glBindVertexArray(mesh->vao);

            active_mesh_indices = mesh->index_count;
        }
        else {
            active_mesh_indices = -1;
        };
    }

    void draw_mesh() {
        if (active_mesh_indices == -1) return;

        //GLint model_loc = glGetUniformLocation(gpu_resources::shaders::default_shader->handle, "model");

        //glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(transform));

        glDrawElements(GL_TRIANGLES, active_mesh_indices, GL_UNSIGNED_INT, nullptr);
    }

    void set_cull_dir(bool front) {
        glCullFace(front ? GL_FRONT : GL_BACK);
    }

    void use_depth(bool depth) {
        glDepthMask(depth ? GL_TRUE : GL_FALSE);
        
        if (depth) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
    }

    void create_uniform_buffer(GLuint& ubo, GLsizeiptr size, int binding) {
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);

        glBufferData(
            GL_UNIFORM_BUFFER,
            size,
            nullptr,
            GL_DYNAMIC_DRAW
        );

        glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);
    }

    GLRenderer() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);

        create_uniform_buffer(camera_ubo.handle, sizeof(CameraUniforms), 0);
    }
};