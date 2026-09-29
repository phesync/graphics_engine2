#include <engine/Input.h>
#include <engine/GLRenderer.h>
#include <engine/Resources.h>
#include <data/WindowCommands.h>
#include <data/Camera.h>
#include <data/GraphicsImage.h>
#include <data/Frustum.h>
#include <data/AABB.h>
#include <data/game/Chunk.h>
#include <data/game/RenderChunk.h>
#include <data/game/BlockInfo.h>
#include <util/Time.h>
#include <util/Map.h>
#include <algorithm/Perlin.h>

#include <imgui.h>

#include <algorithm>
#include <thread>

const std::vector<glm::vec3> adjacent3D = {
    {-1, 0, 0},
    {1, 0, 0},
    {0, 0, 1},
    {0, 0, -1},
    {0, 1, 0},
    {0, -1, 0}
};

void get_chunk_pos(const glm::vec3 world, int16_t& x, int16_t& y, int16_t& z) {
    x = std::floor(world.x / CHUNK_SIZE);
    y = std::floor(world.y / CHUNK_SIZE);
    z = std::floor(world.z / CHUNK_SIZE);
}

uint64_t hash_position(const int16_t x, const int16_t y, const int16_t z) {
    return (static_cast<uint64_t>(static_cast<uint16_t>(x)) << 32) |
        (static_cast<uint64_t>(static_cast<uint16_t>(y)) << 16) |
        static_cast<uint16_t>(z);
}

template <typename T>
uint64_t hash_position(const T& vec3) {
    return hash_position(vec3.x, vec3.y, vec3.z);
}

void dehash_position(const uint64_t& hash, int16_t& x, int16_t& y, int16_t& z)
{
    x = static_cast<int16_t>((hash >> 32) & 0xFFFF);
    y = static_cast<int16_t>((hash >> 16) & 0xFFFF);
    z = static_cast<int16_t>(hash & 0xFFFF);
}

struct Entity {
    ObjTransform transform;
    glm::vec3 velocity;

    Entity() {
        velocity = glm::vec3(0, 0, 0);
    }
};

// Represent a block in world
struct BlockPos {
    int32_t x;
    int32_t y;
    int32_t z;

    BlockPos(double world_x, double world_y, double world_z) :
        x(std::round(world_x)),
        y(std::round(world_y)),
        z(std::round(world_z))
    {
    };

    BlockPos(int block_x, int block_y, int block_z) :
        x(block_x),
        y(block_y),
        z(block_z)
    {
    };

    template <typename T>
    BlockPos(const T& vec3) : BlockPos(vec3.x, vec3.y, vec3.z) {};
};

// Represent a position inside chunk
struct LocalPos {
    uint8_t x;
    uint8_t y;
    uint8_t z;

    template <typename T>
    LocalPos(const T& pos) {
        int r_x = pos.x % CHUNK_SIZE;
        int r_y = pos.y % CHUNK_SIZE;
        int r_z = pos.z % CHUNK_SIZE;

        x = r_x < 0 ? CHUNK_SIZE + r_x : r_x;
        y = r_y < 0 ? CHUNK_SIZE + r_y : r_y;
        z = r_z < 0 ? CHUNK_SIZE + r_z : r_z;
    }
};

// Represent a chunk inside world
struct ChunkPos {
    int16_t x;
    int16_t y;
    int16_t z;

    ChunkPos(double x, double y, double z) :
        x(std::floor((double)x / CHUNK_SIZE)),
        y(std::floor((double)y / CHUNK_SIZE)),
        z(std::floor((double)z / CHUNK_SIZE))
    {
    };

    template <typename T>
    ChunkPos(const T& vec3) : ChunkPos(vec3.x, vec3.y, vec3.z)
    {
    };
};

class Game {
private:
	GLRenderer renderer;

    double last_update = get_time();

    // Camera
    Camera camera;

    float camera_speed = 5;
    float camera_sensitivity = 0.15;

    glm::vec2 camera_rot = { 0, 0 };

    double min_yaw = -90;
    double max_yaw = 90;
    //

    int load_distance = 8;
    int unload_mesh_distance = 8;
    int view_distance = 8;

    std::unordered_map<uint64_t, Chunk> chunks;
    std::unordered_map<uint64_t, RenderChunk> render_chunks;

    bool wireframe_enabled = false;
    bool mouse_locked = true;
    bool is_mouse_locked = false;

    int max_fps = 60;

    // Environment
    float ambient_col[3] = { 0.6, 0.6, 0.6 };
    float sunlight_col[3] = { 0.8, 0.8, 1 };
    float skybox_col_top[3] = { 0.5, 0.8, 1 };
    float skybox_col_bottom[3] = { 0.3, 0.5, 1 };
    float sunlight_intensity = 0.62;
    //

    // Shader Uniforms
    gpu::ShaderUniform u_skybox_col_top = gpu_resources::shaders::skybox_shader->get_uniform("skybox_col_top");
    gpu::ShaderUniform u_skybox_col_bottom = gpu_resources::shaders::skybox_shader->get_uniform("skybox_col_bottom");

    gpu::ShaderUniform u_light_dir = gpu_resources::shaders::default_shader->get_uniform("light_dir");
    gpu::ShaderUniform u_ambient_col = gpu_resources::shaders::default_shader->get_uniform("ambient_col");
    gpu::ShaderUniform u_sunlight_col = gpu_resources::shaders::default_shader->get_uniform("sunlight_col");
    gpu::ShaderUniform u_sunlight_intensity = gpu_resources::shaders::default_shader->get_uniform("sunlight_intensity");
    gpu::ShaderUniform u_camera_pos = gpu_resources::shaders::default_shader->get_uniform("camera_pos");
    gpu::ShaderUniform u_model = gpu_resources::shaders::default_shader->get_uniform("model");
    gpu::ShaderUniform u_world_origin = gpu_resources::shaders::default_shader->get_uniform("world_origin");

    gpu::ShaderUniform u_ui_projection = gpu_resources::shaders::ui_shader->get_uniform("projection");
    gpu::ShaderUniform u_ui_model = gpu_resources::shaders::ui_shader->get_uniform("model");
    //

    Entity player;
    double gravity = 35;
public:
	void update(const Input& input, const int window_w, const int window_h, WindowCommands& win_commands) {
        double current_time = get_time();

        float delta = current_time - last_update;
        float physics_delta = std::min(delta, 0.1f);
        
        last_update = current_time;

        if (input.is_pressed_this_frame(Key::LEFT_ALT)) {
            mouse_locked = !mouse_locked;
        }

        is_mouse_locked = mouse_locked || input.is_pressing(Key::MOUSE_RIGHT);

		win_commands.lock_mouse = is_mouse_locked;

		if (input.is_pressed_this_frame(Key::ESCAPE)) {
			win_commands.close = true;
		}

        if (input.is_pressed_this_frame(Key::R)) {
            wireframe_enabled = !wireframe_enabled;

            renderer.set_wireframe(wireframe_enabled);
        }

        int16_t center_x, center_y, center_z;

        get_chunk_pos(camera.transform.pos, center_x, center_y, center_z);

        // Physics
        //std::cout << "step\n";

        glm::vec3 previous_pos = player.transform.pos;
        AABB previous_plr_bound = AABB::from_transform(previous_pos, player.transform.scale);

        player.transform.pos += player.velocity * physics_delta;

        player.velocity += glm::vec3(0, -gravity * physics_delta, 0);

        BlockPos block_pos(player.transform.pos);

        //AABB player_bound_x_moved = AABB::from_transform(glm::vec3(player.transform.pos.x, previous_pos.y, previous_pos.z), player.transform.scale);
        //AABB player_bound_z_moved = AABB::from_transform(glm::vec3(previous_pos.x, previous_pos.y, player.transform.pos.z), player.transform.scale);

        glm::vec3 new_pos = player.transform.pos;

        bool plr_touching_ground = false;

        AABB player_bound = AABB::from_transform(new_pos, player.transform.scale);

        for (int x = block_pos.x - 2; x < block_pos.x + 2; x++) {
            for (int y = block_pos.y - 2; y < block_pos.y + 2; y++) {
                for (int z = block_pos.z - 2; z < block_pos.z + 2; z++) {
                    BlockPos pos(x, y, z);

                    ChunkPos chunk_pos(pos);
                    LocalPos local_pos(pos);

                    AABB block_bound = AABB::from_transform(
                        glm::vec3(pos.x, pos.y, pos.z),
                        glm::vec3(1, 1, 1)
                    );

                    /*BlockPos block_pos_nx(x - 1, y, z);
                    BlockPos block_pos_px(x + 1, y, z);
                    BlockPos block_pos_nz(x, y, z - 1);
                    BlockPos block_pos_pz(x, y, z + 1);
                    BlockPos block_pos_py(x, y + 1, z);

                    LocalPos local_nx(block_pos_nx);
                    LocalPos local_px(block_pos_px);
                    LocalPos local_nz(block_pos_nz);
                    LocalPos local_pz(block_pos_pz);
                    LocalPos local_py(block_pos_py);

                    bool block_nx = map::get_ptr(chunks, hash_position(ChunkPos(block_pos_nx)))->blocks[local_nx.x][local_nx.y][local_nx.z];
                    bool block_px = map::get_ptr(chunks, hash_position(ChunkPos(block_pos_px)))->blocks[local_px.x][local_px.y][local_px.z];
                    bool block_nz = map::get_ptr(chunks, hash_position(ChunkPos(block_pos_nz)))->blocks[local_nz.x][local_nz.y][local_nz.z];
                    bool block_pz = map::get_ptr(chunks, hash_position(ChunkPos(block_pos_pz)))->blocks[local_pz.x][local_pz.y][local_pz.z];
                    bool block_py = map::get_ptr(chunks, hash_position(ChunkPos(block_pos_py)))->blocks[local_py.x][local_py.y][local_py.z];*/

                    Chunk* chunk = map::get_ptr(chunks, hash_position(chunk_pos.x, chunk_pos.y, chunk_pos.z));

                    if (chunk != nullptr && chunk->blocks[local_pos.x][local_pos.y][local_pos.z] != 0) {

                        if (player_bound.test_aabb(block_bound)) {
                            if (player_bound.min.y <= block_bound.max.y && previous_plr_bound.min.y > block_bound.max.y) {
                                player.velocity = glm::vec3(player.velocity.x, std::max(player.velocity.y, 0.0f), player.velocity.z);
                                new_pos.y = player.transform.scale.y / 2 + block_bound.max.y + 0.0001;

                                plr_touching_ground = true;
                            }

                            if (player_bound.max.x >= block_bound.min.x && previous_plr_bound.max.x < block_bound.min.x) {
                                //player.velocity = glm::vec3(std::min(player.velocity.x, 0.0f), player.velocity.y, player.velocity.z);
                                new_pos.x = block_bound.min.x - player.transform.scale.x / 2 - 0.0001;

                            }

                            if (player_bound.min.x <= block_bound.max.x && previous_plr_bound.min.x > block_bound.max.x) {
                                //player.velocity = glm::vec3(std::max(player.velocity.x, 0.0f), player.velocity.y, player.velocity.z);
                                new_pos.x = block_bound.max.x + player.transform.scale.x / 2 + 0.0001;
                            }

                            if (player_bound.max.z >= block_bound.min.z && previous_plr_bound.max.z < block_bound.min.z) {
                                //player.velocity = glm::vec3(player.velocity.x, player.velocity.y, std::min(player.velocity.z, 0.0f));
                                new_pos.z = block_bound.min.z - player.transform.scale.z / 2 - 0.0001;

                            }

                            if (player_bound.min.z <= block_bound.max.z && previous_plr_bound.min.z > block_bound.max.z) {
                                //player.velocity = glm::vec3(player.velocity.x, player.velocity.y, std::max(player.velocity.z, 0.0f));
                                new_pos.z = block_bound.max.z + player.transform.scale.z / 2 + 0.0001;

                            }
                        };

                    }
                }
            }
        }

        player.transform.pos = new_pos;
             
        //std::cout << "velocity: ";

        //std::cout << player.velocity.y << "\n";

        //Chunk* chunk = map::get_ptr(chunks, hash_position(chunk_pos.x, chunk_pos.y, chunk_pos.z));

        //if (chunk != nullptr && chunk->blocks[relative_pos.x][relative_pos.y][relative_pos.z] != 0) {
            //player.velocity *= glm::vec3(1, 0, 1);
        //}

        /*int16_t player_center_x, player_center_y, player_center_z;

        get_chunk_pos(player.transform.pos, player_center_x, player_center_y, player_center_z);

        int16_t player_relative_x, player_relative_y, player_relative_z;

        player_relative_x = player.transform.pos.x - (player_center_x * CHUNK_SIZE);
        player_relative_y = player.transform.pos.y - (player_center_y * CHUNK_SIZE);
        player_relative_z = player.transform.pos.z - (player_center_z * CHUNK_SIZE);

        Chunk* chunk = map::get_ptr(chunks, hash_position(player_center_x, player_center_y, player_center_z));

        if (chunk != nullptr && chunk->blocks[player_relative_x][player_relative_y][player_relative_z] != 0) {
            player.velocity *= glm::vec3(1, 0, 1);
        }*/

        //

        // Move logic
        glm::vec3 local_move = glm::vec3(0, 0, 0);

        if (input.is_pressing(Key::W)) local_move += glm::vec3(0, 0, -1);

        if (input.is_pressing(Key::A)) local_move += glm::vec3(-1, 0, 0);

        if (input.is_pressing(Key::S)) local_move += glm::vec3(0, 0, 1);

        if (input.is_pressing(Key::D)) local_move += glm::vec3(1, 0, 0);

        //if (input.is_pressing(Key::SPACE)) local_move += glm::vec3(0, 1, 0);

        //if (input.is_pressing(Key::LEFT_SHIFT)) local_move += glm::vec3(0, -1, 0);

        //glm::vec3 move = camera.transform.rot * (local_move * delta);
        //

        if (glm::length(local_move) > 0) {
            local_move = glm::normalize(local_move);
        }

        glm::vec3 player_move = glm::quat(glm::vec3(0, -glm::radians(camera_rot.x), 0)) * (local_move * 5.0f);

        if (plr_touching_ground && input.is_pressing(Key::SPACE)) {
            player.velocity = glm::vec3(player.velocity.x, 9, player.velocity.z);
        }

        glm::vec3 target_velocity = glm::vec3(player_move.x, player.velocity.y, player_move.z);

        glm::vec3 difference = target_velocity - player.velocity;

        float kP = 0.3;

        player.velocity += difference * kP;

        // Camera logic
        camera.aspect = (double)window_w / window_h;

        camera.transform.pos = player.transform.pos + glm::vec3(0, player.transform.scale.y / 2.5, 0);

        if (is_mouse_locked) {
            // Rotate camera

            double rotate_yaw = input.delta_mouse_x * camera_sensitivity;
            double rotate_pitch = (-input.delta_mouse_y) * camera_sensitivity;

            camera_rot.x = std::fmod(camera_rot.x + rotate_yaw, 360.0f);
            camera_rot.y = std::clamp(camera_rot.y + rotate_pitch, min_yaw, max_yaw);

            camera.transform.set_euler(camera_rot.y, -camera_rot.x, 0);
        }
        //

        if (input.is_pressing(Key::P)) {
            BlockPos block_pos(player.transform.pos);
            ChunkPos chunk_pos(block_pos);
            LocalPos relative_pos(block_pos);

            //std::cout << chunk_pos.x << ", " << chunk_pos.z << "\n";

            auto chunk = chunks.find(hash_position(chunk_pos.x, chunk_pos.y, chunk_pos.z));

            chunk->second.blocks[relative_pos.x][relative_pos.y][relative_pos.z] = 1;
            chunk->second.render_version++;
        }

        double loading_time_end = get_time() + 0.01;

        // Load chunks
        for (int x = center_x - load_distance; x <= center_x + load_distance; x++) {
            for (int y = center_y - load_distance; y <= center_y + load_distance; y++) {
                for (int z = center_z - load_distance; z <= center_z + load_distance; z++) {
                    if (get_time() > loading_time_end) break;

                    uint64_t pos = hash_position(x, y, z);

                    if (map::get_ptr(chunks, pos) == nullptr) {
                        chunks[pos] = generate_chunk(x, y, z);

                        for (const glm::vec3 adj : adjacent3D) {
                            Chunk* chunk = map::get_ptr(chunks, hash_position(x + adj.x, y + adj.y, z + adj.z));

                            if (chunk != nullptr) {
                                chunk->render_version++;
                            }
                        }
                    }
                }
            }
        }
        //

        // Delete out of bounds render chunks
        auto rc_it = render_chunks.begin();

        while (rc_it != render_chunks.end()) {
            uint64_t pos = rc_it->first;

            int16_t x, y, z;

            dehash_position(pos, x, y, z);

            bool in_bounds = x >= center_x - unload_mesh_distance && x <= center_x + unload_mesh_distance && y >= center_y - unload_mesh_distance && y <= center_y + unload_mesh_distance && z >= center_z - unload_mesh_distance && z <= center_z + unload_mesh_distance;

            if (!in_bounds) {
                rc_it = render_chunks.erase(rc_it);
            }
            else {
                rc_it++;
            }
        };

        double updating_time_end = get_time() + 0.01;

        // Update render chunks
        for (int x = center_x - view_distance; x <= center_x + view_distance; x++) {
            for (int y = center_y - view_distance; y <= center_y + view_distance; y++) {
                for (int z = center_z - view_distance; z <= center_z + view_distance; z++) {
                    if (get_time() > updating_time_end) break;

                    uint64_t pos = hash_position(x, y, z);

                    Chunk* chunk = map::get_ptr(chunks, pos);

                    if (chunk == nullptr) continue;

                    auto render_chunk_it = render_chunks.find(pos);
                    bool exists = render_chunk_it != render_chunks.end();

                    if (!exists || chunk->render_version != render_chunk_it->second.version) {
                        const Chunk* chunk_nx = map::get_ptr(chunks, hash_position(x - 1, y, z));
                        const Chunk* chunk_px = map::get_ptr(chunks, hash_position(x + 1, y, z));
                        const Chunk* chunk_ny = map::get_ptr(chunks, hash_position(x, y - 1, z));
                        const Chunk* chunk_py = map::get_ptr(chunks, hash_position(x, y + 1, z));
                        const Chunk* chunk_nz = map::get_ptr(chunks, hash_position(x, y, z - 1));
                        const Chunk* chunk_pz = map::get_ptr(chunks, hash_position(x, y, z + 1));

                        if (exists) {
                            render_chunks.erase(render_chunk_it);
                        }

                        render_chunks.try_emplace(
                            pos,
                            *chunk, chunk->render_version, chunk_nx, chunk_px, chunk_ny, chunk_py, chunk_nz, chunk_pz
                        );

                        
                    }
                }
            }
        }
        //

        render(window_w, window_h);

        double next_time = current_time + (1.0f / max_fps);

        while (get_time() < next_time) {
            _mm_pause();
        }
	}

    Chunk generate_chunk(int16_t chunk_x, int16_t chunk_y, int16_t chunk_z) {
        Chunk chunk;

        float frequency_x = 0.01;
        float frequency_y = 0.01;

        float o2_frequency_x = 0.04;
        float o2_frequency_y = 0.04;

        float o3_frequency_x = 0.12;
        float o3_frequency_y = 0.12;

        perlin::PrecomputedPerlin2D terrain_perlin_o1(std::ceil(CHUNK_SIZE * frequency_x + 2), std::ceil(CHUNK_SIZE * frequency_y + 2));
        perlin::PrecomputedPerlin2D terrain_perlin_o2(std::ceil(CHUNK_SIZE * o2_frequency_x + 2), std::ceil(CHUNK_SIZE * o2_frequency_y + 2));
        perlin::PrecomputedPerlin2D terrain_perlin_o3(std::ceil(CHUNK_SIZE * o3_frequency_x + 2), std::ceil(CHUNK_SIZE * o3_frequency_y + 2));

        terrain_perlin_o1.precompute(std::floor(chunk_x * CHUNK_SIZE * frequency_x), std::floor(chunk_z * CHUNK_SIZE * frequency_y));
        terrain_perlin_o2.precompute(std::floor(chunk_x * CHUNK_SIZE * o2_frequency_x), std::floor(chunk_z * CHUNK_SIZE * o2_frequency_y));
        terrain_perlin_o3.precompute(std::floor(chunk_x * CHUNK_SIZE * o3_frequency_x), std::floor(chunk_z * CHUNK_SIZE * o3_frequency_y));

        for (int x = 0; x < CHUNK_SIZE; x++) {
            int world_x = (chunk_x * CHUNK_SIZE) + x;

            for (int z = 0; z < CHUNK_SIZE; z++) {
                int world_z = (chunk_z * CHUNK_SIZE) + z;

                int o1_height = terrain_perlin_o1.get(world_x * frequency_x, world_z * frequency_y) * 40;
                int o2_height = terrain_perlin_o2.get(world_x * o2_frequency_x, world_z * o2_frequency_y) * 15;
                int o3_height = terrain_perlin_o3.get(world_x * o3_frequency_x, world_z * o3_frequency_y) * 10;

                int height = o1_height + o2_height + o3_height;

                for (int y = 0; y < CHUNK_SIZE; y++) {
                    int world_y = (chunk_y * CHUNK_SIZE) + y;

                    if (world_y < height) {
                        chunk.blocks[x][y][z] = 2;
                    }
                    else if (world_y == height) {
                        chunk.blocks[x][y][z] = 1;
                    }
                    else {
                        chunk.blocks[x][y][z] = 0;
                    }
                }
            }
        }

        return chunk;
    }

	void render(const int window_w, const int window_h) {
        renderer.set_viewport(window_w, window_h);
        renderer.clear();

        renderer.set_view(
            camera.get_perspective(), 
            camera.get_view(),
            glm::inverse(glm::mat4(camera.transform.rot))
        );

        // Skybox
        renderer.use_program(gpu_resources::shaders::skybox_shader);

        u_skybox_col_top.set(glm::vec3(skybox_col_top[0], skybox_col_top[1], skybox_col_top[2]));
        u_skybox_col_bottom.set(glm::vec3(skybox_col_bottom[0], skybox_col_bottom[1], skybox_col_bottom[2]));

        renderer.set_cull_dir(true);
        renderer.use_depth(false);

        renderer.use_mesh(gpu_resources::mesh::cube_6);
        renderer.use_texture(gpu_resources::texture::skybox_mask);
        renderer.draw_mesh();

        renderer.set_cull_dir(false);
        renderer.use_depth(true);
        //

        Frustum frustum(camera);

        // Chunks
        renderer.use_program(gpu_resources::shaders::default_shader);

        u_light_dir.set(glm::vec3(0.3, 0.8, 0.4));
        u_ambient_col.set(glm::vec3(ambient_col[0], ambient_col[1], ambient_col[2]));
        u_sunlight_col.set(glm::vec3(sunlight_col[0], sunlight_col[1], sunlight_col[2]));
        u_sunlight_intensity.set(sunlight_intensity);
        u_camera_pos.set(camera.transform.pos);

        renderer.use_texture_array(gpu_resources::texture::terrain);

        for (const auto& [pos, render_chunk] : render_chunks) {
            int16_t x, y, z;

            dehash_position(pos, x, y, z);

            int world_x = x * CHUNK_SIZE;
            int world_y = y * CHUNK_SIZE;
            int world_z = z * CHUNK_SIZE;

            glm::vec3 min(world_x, world_y, world_z);

            AABB bounds(min, min + glm::vec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE));

            if (render_chunk.render_mesh.index_count > 0 && bounds.test_frustum(frustum)) {
                renderer.use_mesh(&render_chunk.render_mesh);

                glm::vec3 world_origin(world_x, world_y, world_z);
                glm::mat4 model = glm::translate(glm::mat4(1.0f), world_origin);

                u_model.set(model);
                u_world_origin.set(world_origin);

                renderer.draw_mesh();
            }
          
        }

        // UI

        renderer.use_program(gpu_resources::shaders::ui_shader);

        float width_half = (float)window_w / 2;
        float height_half = (float)window_h / 2;

        glm::mat4 projection_center = glm::ortho(-width_half, width_half, -height_half, height_half, -1.0f, 1.0f);

        u_ui_projection.set(projection_center);
        u_ui_model.set(glm::scale(glm::mat4(1.0f), glm::vec3(5, 5, 1)));

        renderer.use_depth(false);

        renderer.use_mesh(gpu_resources::mesh::ui_frame);
        renderer.draw_mesh();

        renderer.use_depth(true);

        //

        render_imgui();
	}

    void render_imgui() {
        ImGui::NewFrame();

        ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(500, 240), ImGuiCond_FirstUseEver);
        ImGui::Begin("Panel");

        ImGui::BeginTabBar("Tabs");

        if (ImGui::BeginTabItem("Debug")) {
            BlockPos block_pos(player.transform.pos);
            ChunkPos chunk_pos(block_pos);

            ImGui::Text(std::format("World Pos: {}, {}, {}", block_pos.x, block_pos.y, block_pos.z).c_str());
            ImGui::Text(std::format("Chunk Pos: {}, {}, {}", chunk_pos.x, chunk_pos.y, chunk_pos.z).c_str());
            ImGui::Text(std::format("Chunks Loaded: {}", chunks.size()).c_str());
            ImGui::Text(std::format("Chunk Mesh Loaded: {}", render_chunks.size()).c_str());

            ImGui::EndTabItem();
        };

        if (ImGui::BeginTabItem("Graphics")) {
            ImGui::Text("Environment");
            ImGui::ColorEdit3("Ambient", ambient_col);
            ImGui::ColorEdit3("Sunlight Color", sunlight_col);
            ImGui::DragFloat("Sunlight Intensity", &sunlight_intensity, 0.01, 0, 4);

            ImGui::Text("Skybox");
            ImGui::ColorEdit3("Skybox Color Top", skybox_col_top);
            ImGui::ColorEdit3("Skybox Color Bottom", skybox_col_bottom);

            ImGui::EndTabItem();
        };

        ImGui::EndTabBar();

        ImGui::End();

        /*ImGui::Text(std::format("Position: {}, {}, {}", player.transform.pos.x, player.transform.pos.y, player.transform.pos.z).c_str());

        ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(500, 240), ImGuiCond_FirstUseEver);
        ImGui::Begin("Graphics");

        ImGui::Text("Environment");
        ImGui::ColorEdit3("Ambient", ambient_col);
        ImGui::ColorEdit3("Sunlight Color", sunlight_col);
        ImGui::DragFloat("Sunlight Intensity", &sunlight_intensity, 0.01, 0, 4);

        ImGui::Text("Skybox");
        ImGui::ColorEdit3("Skybox Color Top", skybox_col_top);
        ImGui::ColorEdit3("Skybox Color Bottom", skybox_col_bottom);*/

        ImGui::Render();
    }

    Game() :
        camera(90)
    {
        player.transform.pos = glm::vec3(0, 25, 0);
        player.transform.scale = glm::vec3(0.8f, 1.8f, 0.8f);
    };
};