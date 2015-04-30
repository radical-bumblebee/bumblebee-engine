#ifndef _BulletComponent_H_
#define _BulletComponent_H_

#include "Dependencies/include/bullet/btBulletDynamicsCommon.h"
#include "IComponent.h"

#include <functional>

class BulletComponent : public IComponent {
public:
	typedef std::shared_ptr<BulletComponent> ptr;
	typedef std::unique_ptr<BulletComponent> uptr;

	typedef std::function<void()> expiration_callback_type;

	BulletComponent() : mass(10.0),
						local_inertia(btScalar(0), btScalar(0), btScalar(0)),
						shape(nullptr),
						motion_state(nullptr),
						body(nullptr)
						{ this->type = BULLET_COMPONENT; };
	virtual ~BulletComponent() {
		delete shape;
		motion_state.reset();
		body.reset();
	};

	btCollisionShape* shape;
	std::shared_ptr<btDefaultMotionState> motion_state;
	std::shared_ptr<btRigidBody> body;
	expiration_callback_type expiration_callback;

	btScalar mass;
	btVector3 local_inertia;
};

#endif