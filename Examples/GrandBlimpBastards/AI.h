#ifndef _AI_H_
#define _AI_H_

#include "ISystem.h"
#include "BlimpFactory.h"
#include "SceneInfo.h"
#include <memory>
#include <random>

#define SPAWN_TIMER = 6.0f
class AI : public ISystem {
public:
	typedef std::shared_ptr<AI> ptr;

	AI() {
		spawn_timer = 10.0f;
		fire_timer = 3.0f;
	};
	virtual ~AI() { };

	virtual bool init(BumblebeeWorld::ptr world);
	virtual void destroy();
	virtual void tick();

	glm::vec3 add_cannon_variance(glm::vec3 position, float variance);

	float spawn_timer;
	float spawn_setting;
	float fire_timer;
	float difficulty_setting;//higher it is the harder the game becomes
private:
	
};

#endif