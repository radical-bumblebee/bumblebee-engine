#version 330
#define MAX_LIGHTS 10

struct light_desc {
	vec4 position;
	vec4 intensities;
	vec4 cone_direction;
	float attenuation;
	float ambient;
	float cone_angle;
};

uniform light_desc lights[MAX_LIGHTS];
uniform int num_lights;
uniform float object_shininess;
uniform float object_transparency;
uniform vec4 object_color;
uniform vec4 camera_position;
uniform int render_mode;
uniform float game_over;

uniform sampler2D sampler;
uniform samplerCube cube_sampler;
 
in vec4 ex_position;
in vec4 ex_normal;
in vec4 ex_UV;

out vec4 out_color;

void main() {
	vec4 surface_to_camera = normalize(camera_position - ex_position);
	vec4 linear_color = vec4(0);
	vec4 surface_color = texture(sampler, ex_UV.xy);

	if (surface_color.x == 0 && surface_color.y == 0 && surface_color.z == 0) {
		surface_color = object_color;
	}

	for (int i = 0; i < min(num_lights, MAX_LIGHTS); ++i) {
		vec4 surface_to_light = lights[i].position - ex_position;
		float attenuation = 1.0;
		if (lights[i].position.w == 0) {
			// directional light
			surface_to_light = normalize(lights[i].position);
			attenuation = 1.0;
		}
		else {
			// point light
			surface_to_light = normalize(lights[i].position - ex_position);
			float distance_to_light = length(lights[i].position - ex_position);
			attenuation = 1.0 / (1.0 + lights[i].attenuation * pow(distance_to_light, 2));

			// cone restrictions
			float angle = degrees(acos(dot(-surface_to_light, normalize(lights[i].cone_direction))));
			if (angle > lights[i].cone_angle) {
				attenuation = 0.0;
			}
		}

		// ambient
		vec4 ambient = lights[i].ambient * surface_color * lights[i].intensities;

		// diffuse
		float diffuse_coefficient = max(0.1, dot(ex_normal, surface_to_light));
		vec4 diffuse = diffuse_coefficient * surface_color * lights[i].intensities;

		// specular
		float specular_coefficient = 0.0;
		if (diffuse_coefficient > 0.0) {
			specular_coefficient = pow(max(0.1, dot(surface_to_camera, reflect(-surface_to_light, ex_normal))), object_shininess);
		}
		vec4 specular = specular_coefficient * surface_color * lights[i].intensities;
		vec4 lighting = ambient + attenuation * (diffuse + specular);
		linear_color += lighting;
	}

	vec4 gamma = vec4(1.0 / 2.2);

	out_color = vec4(pow(linear_color, gamma));

	//out_color = mix(linear_color, tex);
	//out_color = vec4(1.0f);

	if (render_mode == 2) {
		out_color = texture(cube_sampler, ex_position.xyz);
		out_color.r *= object_transparency;
		out_color *= object_shininess;
	}

	out_color *= game_over;

	if (render_mode == 1) {
		out_color = vec4(1, 1, 1, texture2D(sampler, ex_position.zw).r) * object_color;
	}

}