#ifndef _BumblebeeObject_H_
#define _BumblebeeObject_H_

#include "BulletComponent.h"
#include "SpatialComponent.h"
#include "ModelComponent.h"
#include "ParticleComponent.h"
#include "AIComponent.h"

#include <memory>
#include <map>

class BumblebeeObject {
public:
	enum State {
		ACTIVE,
		INACTIVE,
		UNUSED
	};

	typedef std::shared_ptr<BumblebeeObject> ptr;
	typedef std::unique_ptr<BumblebeeObject> uptr;

	BumblebeeObject();
	~BumblebeeObject();

	BumblebeeObject(const BumblebeeObject& rhs);
	BumblebeeObject& operator=(const BumblebeeObject& rhs);

	BumblebeeObject(BumblebeeObject&& rhs);
	BumblebeeObject &operator=(BumblebeeObject&& rhs);	

	SpatialComponent::ptr spatial;
	ModelComponent::ptr model;
	BulletComponent::ptr bullet;
	ParticleComponent::ptr particle;
	AIComponent::ptr ai;

	State state;
};

#endif