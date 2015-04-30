#ifndef _BumblebeeRoot_H_
#define _BumblebeeRoot_H_

#include "BumblebeeWorld.h"
#include "SystemTypes.h"
#include "BumblebeeRenderer.h"
#include "BumblebeeBullet.h"

#include "Logger.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <sstream>

// Manages scenes and systems, interfaces with main
class BumblebeeRoot {
public:
	typedef std::unique_ptr<BumblebeeRoot> uptr;

	typedef std::function<void(SDL_Event)> input_callback_type;
	typedef std::function<void(float)> ai_callback_type;

	BumblebeeRoot() { };
	~BumblebeeRoot() { };

	bool init();
	void destroy();

	void loop();
	void tick();

	void register_system(SystemType type);
	void unregister_system(SystemType type);
	void register_custom_system(ISystem::ptr system);

	void register_input_callback(input_callback_type callback);
	void register_ai_callback(ai_callback_type callback);
	void register_collision_callback(BumblebeeBullet::collision_callback_type callback);

	BumblebeeWorld::ptr world;

private:
	std::vector<ISystem::ptr> _systems;
	BumblebeeRenderer::ptr _renderer;
	BumblebeeBullet::ptr _physics;
	input_callback_type _input_callback;
	ai_callback_type _ai_callback;
	
};

#endif