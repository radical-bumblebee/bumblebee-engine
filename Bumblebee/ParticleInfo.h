#ifndef _ParticleInfo_H_
#define _ParticleInfo_H_

#include <memory>

class ParticleInfo {
public:
	typedef std::shared_ptr<ParticleInfo> ptr;

	ParticleInfo() : max_particles(0), elapsed_time(0.0f), swap_pass(false) { };
	~ParticleInfo() { };

	unsigned int vao_id;
	unsigned int particle_buffer[2];
	unsigned int transform_feedback[2];
	unsigned int texture_id;

	unsigned int max_particles;
	float elapsed_time;
	bool swap_pass;

};

#endif