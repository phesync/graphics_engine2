#version 330
uniform vec3 skybox_col_top;
uniform vec3 skybox_col_bottom;
uniform sampler2D uTexture;
in vec2 uv;
out vec4 frag_color;
void main()
{
	float factor = texture(uTexture, uv)[0];

	frag_color = vec4(mix(skybox_col_bottom, skybox_col_top, factor), 1.0);
}