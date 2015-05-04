#include "BumblebeeCamera.h"

// Rotate camera around y axis
void BumblebeeCamera::rotate_radians(float r) {
	this->rotation -= r;
	float radians = r * (static_cast<float>(glm::pi<float>()) / 180.0f);
	float distanceToTarget = -glm::distance(glm::vec3(this->eye.x, this->eye.y, this->eye.z), glm::vec3(this->target.x, this->eye.y, this->target.z));
	this->eye.x = this->target.x + (sin(radians) * distanceToTarget);
	this->eye.z = this->target.z + (cos(radians) * distanceToTarget);
}

// simulates third person camera
void BumblebeeCamera::third_person_view(glm::vec3 camera_target) {
	target = camera_target;
	if (eye.y < max_cam_height) {
		eye.y = (15.0f * eye.y + target.y + camera_height) / 16.0f;
	}

	eye.z = (15.0f * eye.z + target.z) / 16.0f;
	glm::vec3 cam_to_object = target - eye;

	// keep distance between min and max distance
	float cam_dis = glm::length(cam_to_object);
	float correction_factor = 0.0f;
	if (cam_dis < min_cam_dis) {
		correction_factor = 0.95f*(min_cam_dis - cam_dis) / cam_dis;
	}
	if (cam_dis > max_cam_dis) {
		correction_factor = 0.95f*(max_cam_dis - cam_dis) / cam_dis;
	}
	eye -= correction_factor * cam_to_object;
}