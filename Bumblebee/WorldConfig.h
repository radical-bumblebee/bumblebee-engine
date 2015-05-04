#ifndef _WorldConfig_H_
#define _WorldConfig_H_

#include <memory>

// Holds config data about world
class WorldConfig {
public:
	typedef std::shared_ptr<WorldConfig> ptr;

	WorldConfig() : active_scene(0), game_over(1.0f) { };
	~WorldConfig() { };

	unsigned int active_scene;
	float game_over;
};

#endif