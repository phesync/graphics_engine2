#version 330
uniform mat4 model;
uniform mat4 projection;
layout(location = 0) in vec3 vPos;
layout(location = 2) in vec2 vTex;
out vec2 uv;
void main()
{
    gl_Position = model * projection * vec4(vPos, 1.0);
    uv = vTex;
}