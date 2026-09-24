#version 330
layout(std140) uniform Camera
{
    mat4 projection;
    mat4 view;
    mat4 view_rotation;
};
uniform mat4 model;
uniform vec3 world_origin;
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTex;
layout(location = 3) in uint vLayer;
out vec3 normal;
out vec2 uv;
out vec3 world_pos;
flat out uint layer;
void main()
{
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    normal = vNormal;
    uv = vTex;
    layer = vLayer;
    world_pos = world_origin + vPos;
}