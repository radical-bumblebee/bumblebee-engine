#ifndef _SpatialComponent_H_
#define _SpatialComponent_H_

#include "IComponent.h"

#include "Dependencies/include/glm/vec3.hpp"
#include "Dependencies/include/glm/gtc/quaternion.hpp"

// Contains spatial data
class SpatialComponent : public IComponent {
public:
	typedef std::shared_ptr<SpatialComponent> ptr;
	typedef std::unique_ptr<SpatialComponent> uptr;

	SpatialComponent() { 
		this->type = SPATIAL_COMPONENT; 
	};
	SpatialComponent(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f), glm::fquat orientation = glm::fquat()) : position(position), scale(scale), orientation(orientation) {
		this->type = SPATIAL_COMPONENT;
	};
	virtual ~SpatialComponent() { };

	glm::vec3 position;
	glm::vec3 scale;
	glm::fquat orientation;
};

#endif