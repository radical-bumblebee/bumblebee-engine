#ifndef _BumblebeeBullet_H_
#define _BumblebeeBullet_H_

#include "ISystem.h"

#include "Dependencies/include/bullet/btBulletDynamicsCommon.h"

#include <memory>

class BumblebeeBullet : public ISystem {
public:
	typedef std::shared_ptr<BumblebeeBullet> ptr;

	typedef std::function<void(unsigned int, unsigned int, glm::vec3)> collision_callback_type;


	BumblebeeBullet() { };
	virtual ~BumblebeeBullet() { };

	virtual bool init(BumblebeeWorld::ptr world);
	virtual void destroy();
	virtual void tick();

	static void simulation_tick(btDynamicsWorld* bullet_world, btScalar dt);

	// callbacks
	void scene_changed(BumblebeeScene* previous_scene);
	void object_added(unsigned int id);
	void object_removed(unsigned int id);

	collision_callback_type collision_callback;

private:
	btDiscreteDynamicsWorld* _bullet_world;

	btDefaultCollisionConfiguration* _collision_config;
	btCollisionDispatcher* _dispatcher;
	btBroadphaseInterface* _broadphase;
	btSequentialImpulseConstraintSolver* _solver;

};

#endif