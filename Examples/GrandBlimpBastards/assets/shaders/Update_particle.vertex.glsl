#version 330

layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_velocity;
layout(location=2) in float in_type;
layout(location=3) in float in_age;

out vec3 gs_position;
out vec3 gs_velocity;
out float gs_type;
out float gs_age;

void main() {
	gs_position = in_position;
	gs_velocity = in_velocity;
	gs_type = in_type;
	gs_age = in_age;	
}