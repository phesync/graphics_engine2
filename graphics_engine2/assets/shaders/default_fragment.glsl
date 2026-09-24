#version 330
uniform vec3 light_dir;
uniform vec3 ambient_col;
uniform vec3 sunlight_col;
uniform float sunlight_intensity;
uniform vec3 camera_pos;
uniform sampler2DArray uTexture;
in vec3 normal;
in vec2 uv;
in vec3 world_pos;
flat in uint layer;
out vec4 frag_color;
void main()
{
    float sunlight = max(dot(normal, light_dir), 0) * sunlight_intensity;
    float distance = length(world_pos - camera_pos);

    frag_color = texture(uTexture, vec3(uv, layer)) * (vec4(ambient_col, 1.0) + vec4(vec3(sunlight) * sunlight_col, 1.0));
}