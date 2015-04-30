#include "BumblebeeWorld.h"

// Adds a callback for when the scene changes
void BumblebeeWorld::register_scene_changed_callback(callback_type callback) {
	_callbacks.push_back(callback);
}

BumblebeeScene* BumblebeeWorld::scene() {
	return _active_scene;
}

// Changes scene
void BumblebeeWorld::activate_scene(unsigned int id) {
	auto previous_scene = _active_scene;
	_active_scene = _scenes->at(id);
	for (auto callback : _callbacks) {
		// Signals all registered callbacks
		callback(previous_scene);
	}
}

// Adds new scene
unsigned int BumblebeeWorld::create_scene() {
	BumblebeeScene::ptr scene = std::make_shared<BumblebeeScene>();
	scene->init();
	return _scenes->add(scene);
}

// Removes a scene
void BumblebeeWorld::remove_scene(unsigned int id) {
	_scenes->at(id)->destroy();
	_scenes->remove(id);
}

bool BumblebeeWorld::init() {
	_scenes = std::make_shared<BumblebeeContainer<BumblebeeScene>>();
	_active_scene = nullptr; 

	config = std::make_shared<WorldConfig>();
	proxy = std::make_shared<BumblebeeProxy>();
	proxy->init();

	return true;
}

void BumblebeeWorld::destroy() {
	proxy->destroy();
	proxy.reset();
	config.reset();

	_active_scene = nullptr;
	_scenes.reset();
}