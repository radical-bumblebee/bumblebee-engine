#version 330

layout(location=0) in vec4 in_position;
layout(location=1) in vec4 in_normal;
layout(location=2) in vec4 in_UV;

uniform mat4 world_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform int render_mode;

out vec4 ex_position;
out vec4 ex_normal;
out vec4 ex_UV;

void main() {
	mat4 model_view_matrix = view_matrix * world_matrix;
	mat4 model_view_projection_matrix = projection_matrix * model_view_matrix;

	mat4 normal_matrix = transpose(inverse(world_matrix));

	ex_position = world_matrix * in_position;
	ex_normal = normalize(normal_matrix * in_normal);
	ex_UV = in_UV;
	gl_Position = model_view_projection_matrix * in_position;

	if (render_mode == 1) {
		ex_position = in_position;
		gl_Position = vec4(in_position.xy, 0, 1);
	}
}