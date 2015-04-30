#include "BumblebeeParticleRenderer.h"
#include <random>

void BumblebeeParticleRenderer::tick() {
	for (auto obj = _world->scene()->objects->begin(); obj != _world->scene()->objects->end(); ++obj) {
		if (obj->particle) {
			if (obj->particle->visible) {
				obj->particle->info->elapsed_time += _world->dt;

				// Update pass
				_program[1]->enable();
				_program[1]->set_float("delta_time", _world->dt);
				_program[1]->set_float("elapsed_time", obj->particle->info->elapsed_time);

				if (obj->spatial) {
					glm::vec4 particle_origin = glm::vec4(obj->spatial->position, 1.0f) - (obj->particle->particle_offset);
					//particle_origin *= 0.5f;
					_program[1]->set_vec4("particle_origin", glm::value_ptr(particle_origin));
				}

				glEnable(GL_RASTERIZER_DISCARD);

				glBindVertexArray(obj->particle->info->vao_id);
				glBindBuffer(GL_ARRAY_BUFFER, obj->particle->info->particle_buffer[obj->particle->info->swap_pass]);
				glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, obj->particle->info->transform_feedback[!obj->particle->info->swap_pass]);
				glBindTexture(GL_TEXTURE_1D, _random_texture);

				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);
				glEnableVertexAttribArray(3);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(particle), (const GLvoid*)0); // position
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(particle), (const GLvoid*)(sizeof(float) * 3)); // velocity
				glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(particle), (const GLvoid*)(sizeof(float) * 6)); // type
				glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(particle), (const GLvoid*)(sizeof(float) * 7)); // age

				glBeginTransformFeedback(GL_POINTS);
				if (_first) {
					glDrawArrays(GL_POINTS, 0, 1);
					_first = false;
				}
				else {
					glDrawTransformFeedback(GL_POINTS, obj->particle->info->transform_feedback[obj->particle->info->swap_pass]);
				}
				glEndTransformFeedback();

				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
				glDisableVertexAttribArray(3);
				glBindTexture(GL_TEXTURE_1D, 0);

				// Render pass
				glDisable(GL_RASTERIZER_DISCARD);
				glEnable(GL_BLEND);

				_program[0]->enable();
				_program[0]->set_matrix("view_projection", glm::value_ptr(_world->projection_matrix * _world->scene()->camera()->view_matrix), false);
				_program[0]->set_vec4("camera_position", glm::value_ptr(_world->scene()->camera()->eye));

				glBindBuffer(GL_ARRAY_BUFFER, obj->particle->info->particle_buffer[!obj->particle->info->swap_pass]);
				glBindTexture(GL_TEXTURE_2D, obj->particle->info->texture_id);

				glDrawTransformFeedback(GL_POINTS, obj->particle->info->transform_feedback[!obj->particle->info->swap_pass]);

				glDisable(GL_BLEND);
				glDisableVertexAttribArray(0);
				glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);

				// Swap buffers
				obj->particle->info->swap_pass = !obj->particle->info->swap_pass;
			}
		}
	}
}

void BumblebeeParticleRenderer::particle_added(ParticleInfo::ptr info) {
	particle* particles = new particle[info->max_particles];
	memset(particles, 0, sizeof(particles));

	glGenVertexArrays(1, &info->vao_id);
	glGenTransformFeedbacks(2, info->transform_feedback);
	glGenBuffers(2, info->particle_buffer);

	for (int i = 0; i < 2; ++i) {
		glBindVertexArray(info->vao_id);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, info->transform_feedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, info->particle_buffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle) * info->max_particles, (const GLvoid*)particles, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, info->particle_buffer[i]);
	}

	delete[] particles;
}

bool BumblebeeParticleRenderer::init_shaders() {	
	// Particle render pass
	_program[0] = std::make_shared<BumblebeeGLSLProgram>();
	if (!_program[0]->set_shader_source(VERTEX_SHADER, "assets/shaders/Render_particle.vertex.glsl")) {
		std::cout << "Error setting vertex shader source" << std::endl;
		return false;
	}
	if (!_program[0]->set_shader_source(GEOMETRY_SHADER, "assets/shaders/Render_particle.geometry.glsl")) {
		std::cout << "Error setting vertex shader source" << std::endl;
		return false;
	}
	if (!_program[0]->set_shader_source(FRAGMENT_SHADER, "assets/shaders/Render_particle.fragment.glsl")) {
		std::cout << "Error setting fragment shader source" << std::endl;
		return false;
	}
	if (!_program[0]->compile()) {
		std::cout << "Error compiling shaders" << std::endl;
		return false;
	}

	// Particle update pass
	_program[1] = std::make_shared<BumblebeeGLSLProgram>();
	if (!_program[1]->set_shader_source(VERTEX_SHADER, "assets/shaders/Update_particle.vertex.glsl")) {
		std::cout << "Error setting vertex shader source" << std::endl;
		return false;
	}
	if (!_program[1]->set_shader_source(GEOMETRY_SHADER, "assets/shaders/Update_particle.geometry.glsl")) {
		std::cout << "Error setting vertex shader source" << std::endl;
		return false;
	}
	if (!_program[1]->set_shader_source(FRAGMENT_SHADER, "assets/shaders/Update_particle.fragment.glsl")) {
		std::cout << "Error setting fragment shader source" << std::endl;
		return false;
	}

	if (!_program[1]->compile(true)) {
		std::cout << "Error compiling shaders" << std::endl;
		return false;
	}

	return true;
}

void BumblebeeParticleRenderer::scene_changed(BumblebeeScene* previous_scene) {
	if (previous_scene) {
		for (auto particle : _world->proxy->particles) {
			if (particle.second) {

			}
		}
	}
}

bool BumblebeeParticleRenderer::init(BumblebeeWorld::ptr world) {
	_world = world;
	_elapsed_time = 0.0f;

	init_shaders();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-1.0f, 1.0f);

	int random_size = 200;
	float* random_data = new float[random_size*3];
	auto walk = random_data;
	for (int i = 0; i < random_size; ++i) {
		*walk = (float)dis(gen);
		++walk;
		*walk = (float)dis(gen);
		++walk;
		*walk = (float)dis(gen);
		++walk;
	}

	glGenTextures(1, &_random_texture);
	glBindTexture(GL_TEXTURE_1D, _random_texture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, random_size, 0, GL_RGB, GL_FLOAT, random_data);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glBindTexture(GL_TEXTURE_1D, 0);

	delete[] random_data;

	_world->proxy->particle_added_callback = std::bind(&BumblebeeParticleRenderer::particle_added, this, std::placeholders::_1);
	_world->register_scene_changed_callback(std::bind(&BumblebeeParticleRenderer::scene_changed, this, std::placeholders::_1));

	return true;
}

void BumblebeeParticleRenderer::destroy() {
	_program[1].reset();
	_program[0].reset();
	_world.reset();
}