#ifndef _BumblebeeCamera_H_
#define _BumblebeeCamera_H_

#include "BumblebeeObject.h"

#include "Dependencies/include/glm/vec3.hpp"
#include "Dependencies/include/glm/gtc/quaternion.hpp"
#include "Dependencies/include/glm/gtc/matrix_transform.hpp"

#include <memory>

// Camera class, just a basic data container for now
class BumblebeeCamera {
public:
	typedef std::shared_ptr<BumblebeeCamera> ptr;

	BumblebeeCamera() {
		camera_height = 2.f;
		min_cam_dis = 3.f;
		max_cam_dis = 10.f;
		max_cam_height = 14.0f;
	};

	~BumblebeeCamera() { };

	glm::vec3 eye;
	glm::vec3 target;
	glm::vec3 up;
	glm::fquat orientation;
	glm::mat4 view_matrix;

	float camera_height;
	float min_cam_dis;
	float max_cam_dis;
	float max_cam_height;
	void third_person_view(glm::vec3 position);

	float rotation;
	void rotate_radians(float radians);
};

#endif