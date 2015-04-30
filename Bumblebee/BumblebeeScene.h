#ifndef _BumblebeeScene_H_
#define _BumblebeeScene_H_

#include <memory>
#include <vector>

#include "BumblebeeContainer.h"
#include "BumblebeeObject.h"
#include "BumblebeeLight.h"
#include "BumblebeeCamera.h"
#include "BumblebeeUIElement.h"

// Holds scene objects and configuration
class BumblebeeScene {
public:
	typedef std::shared_ptr<BumblebeeScene> ptr;

	BumblebeeScene() { };
	~BumblebeeScene() {	};

	bool init();
	void destroy();

	unsigned int create_camera();
	void remove_camera(unsigned int id);
	void activate_camera(unsigned int id);
	BumblebeeCamera* camera();

	BumblebeeContainer<BumblebeeObject>::ptr objects;
	BumblebeeContainer<BumblebeeLight>::ptr lights;
	BumblebeeContainer<BumblebeeUIElement>::ptr ui;

	unsigned int player_id;

private:
	BumblebeeContainer<BumblebeeCamera>::ptr _cameras;
	BumblebeeCamera* _active_camera;

};

#endif