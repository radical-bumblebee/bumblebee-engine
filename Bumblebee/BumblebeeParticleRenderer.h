#ifndef _BumblebeeParticleRenderer_H_
#define _BumblebeeParticleRenderer_H_

#include "ISystem.h"
#include "BumblebeeGLSLProgram.h"
#include "ParticleInfo.h"

struct particle {
	float type;
	float age;
	glm::vec3 position;
	glm::vec3 velocity;
};

// Manages particle rendering using modern opengl
class BumblebeeParticleRenderer : public ISystem {
public:
	typedef std::shared_ptr<BumblebeeParticleRenderer> ptr;

	BumblebeeParticleRenderer() : _first(true), _swap_pass(false) { };
	virtual ~BumblebeeParticleRenderer() { };

	virtual bool init(BumblebeeWorld::ptr world);
	virtual void destroy();
	virtual void tick();

	bool init_shaders();
	void particle_added(ParticleInfo::ptr info);

	void scene_changed(BumblebeeScene* previous_scene);

private:
	BumblebeeGLSLProgram::ptr _program[2];

	GLuint _random_texture;
	GLuint _particle_buffer[2];
	GLuint _transform_feedback[2];
	GLuint _vao_id;
	bool _swap_pass;
	bool _first;
	float _elapsed_time;

};

#endif