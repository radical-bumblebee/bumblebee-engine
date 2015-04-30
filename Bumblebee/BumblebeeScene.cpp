#include "BumblebeeScene.h"

BumblebeeCamera* BumblebeeScene::camera() {
	return _active_camera;
};

void BumblebeeScene::activate_camera(unsigned int id) {
	_active_camera = _cameras->at(id);
}

unsigned int BumblebeeScene::create_camera() {
	return _cameras->add(std::make_shared<BumblebeeCamera>());
}

void BumblebeeScene::remove_camera(unsigned int id) {
	_cameras->remove(id);
}

bool BumblebeeScene::init() {
	objects = std::make_shared<BumblebeeContainer<BumblebeeObject>>();
	lights = std::make_shared<BumblebeeContainer<BumblebeeLight>>();
	ui = std::make_shared<BumblebeeContainer<BumblebeeUIElement>>();
	_cameras = std::make_shared<BumblebeeContainer<BumblebeeCamera>>();
	player_id = 0;

	activate_camera(create_camera());
	camera()->eye = glm::vec3(20.0f, 13.0f, -6.0f);
	camera()->target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera()->up = glm::vec3(0.0f, 1.0f, 0.0f);

	return true;
}

void BumblebeeScene::destroy() {
	_cameras.reset();
	ui.reset();
	lights.reset();
	objects.reset();
}
