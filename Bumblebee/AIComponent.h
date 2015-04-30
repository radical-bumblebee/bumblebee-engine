#ifndef _AIComponent_H_
#define _AIComponent_H_

#include "IComponent.h"

// Contains model data
class AIComponent : public IComponent {
public:
	enum AIGroup {
		PLAYER,
		ENEMY,
		NONE
	};

	enum AIWeapon {
		GUN,
		RAM
	};

	typedef std::shared_ptr<AIComponent> ptr;
	typedef std::unique_ptr<AIComponent> uptr;

	AIComponent() : ai_group(PLAYER) {
		ai_group = ENEMY;
		this->type = AI_COMPONENT;
	}
	AIComponent(AIGroup group) : ai_group(group) {
		this->type = AI_COMPONENT;
	}
	virtual ~AIComponent() {
	};

	AIGroup ai_group;
	AIWeapon ai_weapon;
};

#endif