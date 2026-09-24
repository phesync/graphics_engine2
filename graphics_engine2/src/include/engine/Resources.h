#pragma once

#include <data/opengl/Mesh.h>
#include <data/opengl/Texture.h>
#include <data/opengl/Shader.h>
#include <data/opengl/ShaderProgram.h>
#include <data/opengl/TextureArray.h>

#include <data/game/BlockInfo.h>

namespace data_resources {
	namespace mesh {
        MeshData cube = {
            {
                // front
                { { -0.5, -0.5, 0.5 }, {0, 0, 1}, {0, 0} },
                { {  0.5, -0.5, 0.5 }, {0, 0, 1}, {1, 0} },
                { {  0.5,  0.5, 0.5 }, {0, 0, 1}, {1, 1} },
                { { -0.5,  0.5, 0.5 }, {0, 0, 1}, {0, 1} },

                // back
                { { -0.5, -0.5, -0.5 }, {0, 0, -1}, {1, 0} },
                { {  0.5, -0.5, -0.5 }, {0, 0, -1}, {0, 0} },
                { {  0.5,  0.5, -0.5 }, {0, 0, -1}, {0, 1} },
                { { -0.5,  0.5, -0.5 }, {0, 0, -1}, {1, 1} },

                // top
                { { -0.5,  0.5, 0.5 }, {0, 1, 0}, {0, 0} },
                { {  0.5,  0.5, 0.5 }, {0, 1, 0}, {1, 0} },
                { {  0.5,  0.5, -0.5 }, {0, 1, 0}, {1, 1} },
                { { -0.5,  0.5, -0.5 }, {0, 1, 0}, {0, 1} },

                // bottom
                { { -0.5,  -0.5, 0.5 }, {0, -1, 0}, {0, 1} },
                { {  0.5,  -0.5, 0.5 }, {0, -1, 0}, {1, 1} },
                { {  0.5,  -0.5, -0.5 }, {0, -1, 0}, {1, 0} },
                { { -0.5,  -0.5, -0.5 }, {0, -1, 0}, {0, 0} },

                // left
                { { -0.5,  -0.5, 0.5 }, {-1, 0, 0}, {1, 0} },
                { { -0.5,  0.5, 0.5 }, {-1, 0, 0}, {1, 1} },
                { { -0.5,  0.5, -0.5 }, {-1, 0, 0}, {0, 1} },
                { { -0.5,  -0.5, -0.5 }, {-1, 0, 0}, {0, 0} },

                // right
                { { 0.5,  -0.5, 0.5 }, {1, 0, 0}, {0, 0} },
                { { 0.5,  0.5, 0.5 }, {1, 0, 0}, {0, 1} },
                { { 0.5,  0.5, -0.5 }, {1, 0, 0}, {1, 1} },
                { { 0.5,  -0.5, -0.5 }, {1, 0, 0}, {1, 0} },
            },
            {
                0, 1, 2, 0, 2, 3,
                4, 6, 5, 4, 7, 6,
                8, 9, 10, 8, 10, 11,
                12, 14, 13, 12, 15, 14,
                16, 17, 18, 16, 18, 19,
                20, 22, 21, 20, 23, 22,
            }
        };
        MeshData cube_6 = {
                {
                // front
                { { -0.5, -0.5, 0.5 }, {0, 0, 1}, {0, 1.0f / 3} },
                { {  0.5, -0.5, 0.5 }, {0, 0, 1}, {0.25, 1.0f / 3} },
                { {  0.5,  0.5, 0.5 }, {0, 0, 1}, {0.25, 2.0f / 3} },
                { { -0.5,  0.5, 0.5 }, {0, 0, 1}, {0, 2.0f / 3} },

                // back
                { { -0.5, -0.5, -0.5 }, {0, 0, -1}, {0.75, 1.0f / 3} },
                { {  0.5, -0.5, -0.5 }, {0, 0, -1}, {0.5, 1.0f / 3} },
                { {  0.5,  0.5, -0.5 }, {0, 0, -1}, {0.5, 2.0f / 3} },
                { { -0.5,  0.5, -0.5 }, {0, 0, -1}, {0.75, 2.0f / 3} },

                // top
                { { -0.5,  0.5, 0.5 }, {0, 1, 0}, {0.25, 2.0f / 3} },
                { {  0.5,  0.5, 0.5 }, {0, 1, 0}, {0.5, 2.0f / 3} },
                { {  0.5,  0.5, -0.5 }, {0, 1, 0}, {0.5, 1} },
                { { -0.5,  0.5, -0.5 }, {0, 1, 0}, {0.25, 1} },

                // bottom
                { { -0.5,  -0.5, 0.5 }, {0, -1, 0}, {0.25, 1.0f / 3} },
                { {  0.5,  -0.5, 0.5 }, {0, -1, 0}, {0.5, 1.0f / 3} },
                { {  0.5,  -0.5, -0.5 }, {0, -1, 0}, {0.5, 0} },
                { { -0.5,  -0.5, -0.5 }, {0, -1, 0}, {0.25, 0} },

                // left
                { { -0.5,  -0.5, 0.5 }, {-1, 0, 0}, {1, 1.0f / 3} },
                { { -0.5,  0.5, 0.5 }, {-1, 0, 0}, {1, 2.0f / 3} },
                { { -0.5,  0.5, -0.5 }, {-1, 0, 0}, {0.75, 2.0f / 3} },
                { { -0.5,  -0.5, -0.5 }, {-1, 0, 0}, {0.75, 1.0f / 3} },

                // right
                { { 0.5,  -0.5, 0.5 }, {1, 0, 0}, {0.25, 1.0f / 3} },
                { { 0.5,  0.5, 0.5 }, {1, 0, 0}, {0.25, 2.0f / 3} },
                { { 0.5,  0.5, -0.5 }, {1, 0, 0}, {0.5, 2.0f / 3} },
                { { 0.5,  -0.5, -0.5 }, {1, 0, 0}, {0.5, 1.0f / 3} },
            },
            {
                0, 1, 2, 0, 2, 3,
                4, 6, 5, 4, 7, 6,
                8, 9, 10, 8, 10, 11,
                12, 14, 13, 12, 15, 14,
                16, 17, 18, 16, 18, 19,
                20, 22, 21, 20, 23, 22,
            }
        };
        MeshData ui_frame = {
            {
                { { -0.5, -0.5, 0 }, {0, 0, 1}, {0, 0} },
                { {  0.5, -0.5, 0 }, {0, 0, 1}, {1, 0} },
                { {  0.5, 0.5, 0 }, {0, 0, 1}, {1, 1} },
                { { -0.5, 0.5, 0 }, {0, 0, 1}, {0, 1} }
            },
            {
                0, 1, 2, 0, 2, 3
            }
        };
	}
}

namespace gpu_resources {
    namespace shaders {
		gpu::Shader* default_vert;
		gpu::Shader* default_frag;

		gpu::Shader* skybox_vert;
		gpu::Shader* skybox_frag;

        gpu::Shader* ui_vert;
        gpu::Shader* ui_frag;

		gpu::ShaderProgram* default_shader;
		gpu::ShaderProgram* skybox_shader;
        gpu::ShaderProgram* ui_shader;
	}

	namespace mesh {
		gpu::Mesh* cube;
		gpu::Mesh* cube_6;
        gpu::Mesh* ui_frame;
	}

    namespace texture {
        gpu::Texture* skybox;
        gpu::Texture* skybox_mask;
        gpu::TextureArray* terrain; // special case
    }

	void load_all() {
		// SHADERS
		shaders::default_vert = new gpu::Shader(file::get_text("assets/shaders/default_vertex.glsl").c_str(), gpu::ShaderType::VERTEX);
		shaders::default_frag = new gpu::Shader(file::get_text("assets/shaders/default_fragment.glsl").c_str(), gpu::ShaderType::FRAGMENT);

		shaders::skybox_vert = new gpu::Shader(file::get_text("assets/shaders/skybox_vertex.glsl").c_str(), gpu::ShaderType::VERTEX);
		shaders::skybox_frag = new gpu::Shader(file::get_text("assets/shaders/skybox_fragment.glsl").c_str(), gpu::ShaderType::FRAGMENT);

        shaders::ui_vert = new gpu::Shader(file::get_text("assets/shaders/ui_vertex.glsl").c_str(), gpu::ShaderType::VERTEX);
        shaders::ui_frag = new gpu::Shader(file::get_text("assets/shaders/ui_fragment.glsl").c_str(), gpu::ShaderType::FRAGMENT);

		shaders::default_shader = new gpu::ShaderProgram({ shaders::default_vert, shaders::default_frag });
		shaders::skybox_shader = new gpu::ShaderProgram({ shaders::skybox_vert, shaders::skybox_frag });
        shaders::ui_shader = new gpu::ShaderProgram({ shaders::ui_vert, shaders::ui_frag });

		// MESH
        mesh::cube = new gpu::Mesh(data_resources::mesh::cube);
        mesh::cube_6 = new gpu::Mesh(data_resources::mesh::cube_6);
        mesh::ui_frame = new gpu::Mesh(data_resources::mesh::ui_frame);

        // TEXTURE
        texture::skybox = new gpu::Texture(GraphicsImage("assets/textures/skybox_2.png"));
        texture::skybox_mask = new gpu::Texture(GraphicsImage("assets/textures/skybox_mask.png"));

        texture::terrain = new gpu::TextureArray(16, 16, 8);

        for (int i = 0; i < Block_Texture.size(); i++) {
            const std::string& path = Block_Texture[i];

            texture::terrain->upload(GraphicsImage(path), i);
        }
	}
}