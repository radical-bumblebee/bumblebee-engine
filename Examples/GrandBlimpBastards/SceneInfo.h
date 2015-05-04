#ifndef _SceneInfo_H_
#define _SceneInfo_H_

#include <memory>
#include <Dependencies/include/glm/vec3.hpp>

#define RESET_PERIOD 0.5f

class SceneInfo {
public:
	typedef std::shared_ptr<SceneInfo> ptr;

	SceneInfo() : rotate(false), line_id(0), cannonball_id(0), scene_id(0), skybox_id(0), scene_timer(0.0f), highest_score(0) { };
	~SceneInfo() { };
	
	unsigned int line_id;
	unsigned int cannonball_id;
	unsigned int skybox_id;
	unsigned int scene_id;

	bool rotate;
	float scene_timer;
	int highest_score;

	glm::vec3 target_direction;
	glm::vec3 cannon_origin;

	const char* cannonball_name = "assets/models/sphere.ply";
	const char* blimp_name = "assets/models/capsule.ply";

};

#endif