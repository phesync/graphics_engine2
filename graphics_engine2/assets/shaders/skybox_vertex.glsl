#version 330
layout(std140) uniform Camera
{
    mat4 projection;
    mat4 view;
    mat4 view_rotation;
};
layout(location = 0) in vec3 vPos;
layout(location = 2) in vec2 vTex;
out vec2 uv;
void main()
{
    gl_Position = projection * view_rotation * vec4(vPos, 1.0);
    uv = vTex;
}