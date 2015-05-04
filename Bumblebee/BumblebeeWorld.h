#ifndef _BumblebeeWorld_H_
#define _BumblebeeWorld_H_

#include "WorldConfig.h"
#include "BumblebeeContainer.h"
#include "BumblebeeScene.h"
#include "BumblebeeProxy.h"

#include <memory>

// Represents all common data in simulation
class BumblebeeWorld {
public:
	typedef std::shared_ptr<BumblebeeWorld> ptr;
	typedef std::function<void(BumblebeeScene*)> callback_type;

	BumblebeeWorld() : dt(0.0f), fps(0.0f) { };
	~BumblebeeWorld() { };

	bool init();
	void destroy();

	void register_scene_changed_callback(callback_type callback);
	BumblebeeScene* scene();
	unsigned int create_scene();
	void remove_scene(unsigned int id);
	void activate_scene(unsigned int id);
	std::pair<float, float> get_window_size();

	BumblebeeProxy::ptr proxy;
	WorldConfig::ptr config;

	float dt;
	float fps;
	float window_w;
	float window_h;
	glm::mat4 projection_matrix;

private:
	BumblebeeContainer<BumblebeeScene>::ptr _scenes;
	std::vector<callback_type> _callbacks;

	BumblebeeScene* _active_scene;
	
};

#endif