#version 330

uniform mat4 view_projection;
uniform vec4 camera_position;
float billboard_size = 1.0f;

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices=4) out;

out vec2 fs_texture_coordinates;

void main() {

	vec3 in_position = gl_in[0].gl_Position.xyz;
	vec3 to_camera = normalize(camera_position.xyz - in_position);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(to_camera, up) * billboard_size;

	in_position -= right;
	gl_Position = view_projection * vec4(in_position, 1.0);
	fs_texture_coordinates = vec2(0.0, 0.0);
	EmitVertex();

	in_position.y += billboard_size;
	gl_Position = view_projection * vec4(in_position, 1.0);
	fs_texture_coordinates = vec2(0.0, 1.0);
	EmitVertex();

	in_position.y -= billboard_size;
	in_position += right;
	gl_Position = view_projection * vec4(in_position, 1.0);
	fs_texture_coordinates = vec2(1.0, 0.0);
	EmitVertex();

	in_position.y += billboard_size;
	gl_Position = view_projection * vec4(in_position, 1.0);
	fs_texture_coordinates = vec2(1.0, 1.0);
	EmitVertex();

	EndPrimitive();
}