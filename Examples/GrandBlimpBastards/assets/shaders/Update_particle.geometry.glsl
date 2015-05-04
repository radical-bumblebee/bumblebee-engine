#version 330

layout(points) in;
layout(points) out;
layout(max_vertices=30) out;

uniform float delta_time;
uniform float elapsed_time;
uniform vec4 particle_origin;
uniform sampler1D random_texture;

in vec3 gs_position[];
in vec3 gs_velocity[];
in float gs_type[];
in float gs_age[];

out vec3 fs_position;
out vec3 fs_velocity;
out float fs_type;
out float fs_age;

#define PARTICLE_ORIGIN 0.0f
#define PARTICLE_GENERATED 1.0f

float origin_lifetime = 0.0001;
float generated_lifetime = 1.0;

vec3 random_direction(float coordinate) {
	vec3 direction = texture(random_texture, coordinate).xyz;

	return direction;
}

void main() {
	float age = gs_age[0] + delta_time;

	if (gs_type[0] == PARTICLE_ORIGIN) {
		if (age >= origin_lifetime) {
			fs_type = PARTICLE_GENERATED;
			fs_age = 0.0;
			vec3 direction = random_direction(elapsed_time);
			direction = normalize(direction);
			direction.z -= 0.5;
			direction.x -= 0.5;

			fs_velocity = direction;


			fs_position = gs_position[0];

			EmitVertex();
			EndPrimitive();

			age = 0.0;
		}

		fs_type = PARTICLE_ORIGIN;
		fs_position = particle_origin.xyz;
		fs_velocity = gs_velocity[0];
		fs_age = age;

		EmitVertex();
		EndPrimitive;
	} 
	else {
		if (gs_type[0] == PARTICLE_GENERATED) {
			if (age < generated_lifetime) {
				vec3 gravity = vec3(0.0, 5.0, 0.0);
				gravity.y *= delta_time;

				fs_type = gs_type[0];
				fs_age = age;
				fs_position = gs_position[0] + (delta_time * gs_velocity[0]);
				fs_velocity = gs_velocity[0] + gravity;
				
				EmitVertex();
				EndPrimitive();
			}
		}
	}
}