#ifndef _BumblebeeLight_H_
#define _BumblebeeLight_H_

#include "LightTypes.h"

#include "Dependencies/include/glm/vec4.hpp"

#include <memory>

// Lighting description
class BumblebeeLight {
public:
	typedef std::shared_ptr<BumblebeeLight> ptr;
	typedef std::unique_ptr<BumblebeeLight> uptr;

	BumblebeeLight() : attenuation(0.0f), ambient(0.0f), cone_angle(0.0f) { };
	BumblebeeLight(LightType type) : type(type), attenuation(0.0f), ambient(0.0f), cone_angle(0.0f) { };
	~BumblebeeLight() { };

	glm::vec4 position;
	glm::vec4 intensities;
	glm::vec4 cone_direction;
	float attenuation;
	float ambient;
	float cone_angle;
	LightType type;
};

#endif