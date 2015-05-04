#version 330

uniform sampler2D sampler;

in vec2 fs_texture_coordinates;

out vec4 out_color;

void main() { 
	out_color = texture2D(sampler, fs_texture_coordinates);
	out_color.a = 0.1;

	if (out_color.r <= 0.4 && out_color.g <= 0.4 && out_color.b <= 0.4) {
		discard;
	}
}