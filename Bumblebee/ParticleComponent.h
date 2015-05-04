#ifndef _ParticleComponent_H_
#define _ParticleComponent_H_

#include "IComponent.h"
#include "ParticleInfo.h"

// Contains particle data
class ParticleComponent : public IComponent {
public:
	typedef std::shared_ptr<ParticleComponent> ptr;

	ParticleComponent() : visible(true) {
		this->type = PARTICLE_COMPONENT;
	};
	virtual ~ParticleComponent() { };

	ParticleInfo::ptr info;
	glm::vec4 particle_offset;
	bool visible;
	
};

#endif