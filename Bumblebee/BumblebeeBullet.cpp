#include "BumblebeeBullet.h"
#include "SpatialComponent.h"
#include "BulletComponent.h"
#include "ModelComponent.h"
#include <iostream>

void BumblebeeBullet::simulation_tick(btDynamicsWorld* bullet_world, btScalar dt) { 
	auto bee = static_cast<BumblebeeBullet*>(bullet_world->getWorldUserInfo());

	for (int i = 0; i < bullet_world->getDispatcher()->getNumManifolds(); ++i) {
		btPersistentManifold* contact_manifold = bullet_world->getDispatcher()->getManifoldByIndexInternal(i);

		const btCollisionObject* obj_a = contact_manifold->getBody0();
		const btCollisionObject* obj_b = contact_manifold->getBody1();

		glm::vec3 contact_point;
		for (int i = 0; i < contact_manifold->getNumContacts(); ++i) {
			btManifoldPoint& pt = contact_manifold->getContactPoint(i);
			glm::vec3 contact_point = glm::vec3(pt.getPositionWorldOnA().x(), pt.getPositionWorldOnA().y(), pt.getPositionWorldOnA().z());
			bee->collision_callback(obj_a->getUserIndex(), obj_b->getUserIndex(), contact_point);
		}		
	}

	for (int i = 0; i < bullet_world->getDispatcher()->getNumManifolds(); ++i) {
		btPersistentManifold* contact_manifold = bullet_world->getDispatcher()->getManifoldByIndexInternal(i);

		const btCollisionObject* obj_a = contact_manifold->getBody0();
		const btCollisionObject* obj_b = contact_manifold->getBody1();

		bullet_world->removeRigidBody(bee->_world->scene()->objects->at(obj_a->getUserIndex())->bullet->body.get());
		bullet_world->addRigidBody(bee->_world->scene()->objects->at(obj_a->getUserIndex())->bullet->body.get());
		bullet_world->removeRigidBody(bee->_world->scene()->objects->at(obj_b->getUserIndex())->bullet->body.get());
		bullet_world->addRigidBody(bee->_world->scene()->objects->at(obj_b->getUserIndex())->bullet->body.get());
	}
}

// Ticks system
void BumblebeeBullet::tick() {
	// Updates physics on all active objects
	_bullet_world->stepSimulation(_world->dt, 7);

	for (auto obj = _world->scene()->objects->begin(); obj != _world->scene()->objects->end(); ++obj) {
		if (obj->bullet) {
			if (obj->bullet->body && obj->bullet->body->getMotionState()) {
				btTransform transform;
				obj->bullet->body->getMotionState()->getWorldTransform(transform);

				if (transform.getOrigin().getY() < -50.0f) {
					if (obj->bullet->expiration_callback) {
						obj->bullet->expiration_callback();
					}
					else {
						obj = _world->scene()->objects->remove(obj.pos);
					}
					continue;
				}

				// Updates objects spatial data
				if (obj->spatial) {
					obj->spatial->position.x = transform.getOrigin().getX();
					obj->spatial->position.y = transform.getOrigin().getY();
					obj->spatial->position.z = transform.getOrigin().getZ();
					obj->spatial->orientation.x = transform.getRotation().x();
					obj->spatial->orientation.y = transform.getRotation().y();
					obj->spatial->orientation.z = transform.getRotation().z();
					obj->spatial->orientation.w = transform.getRotation().w();
				}
			}
		}
	}
}

void BumblebeeBullet::scene_changed(BumblebeeScene* previous_scene) {
	_world->scene()->objects->object_added_callback = std::bind(&BumblebeeBullet::object_added, this, std::placeholders::_1);
	_world->scene()->objects->object_removed_callback = std::bind(&BumblebeeBullet::object_removed, this, std::placeholders::_1);

	// Clear physics world of previous scene
	if (previous_scene) {
		for (auto& obj : *previous_scene->objects) {
			if (obj->bullet) {
				if (obj->bullet->body) {
					_bullet_world->removeRigidBody(obj->bullet->body.get());
				}
			}
		}
	}

	// Re-populate from new scene
	for (auto obj = _world->scene()->objects->begin(); obj != _world->scene()->objects->end(); ++obj) {
		_bullet_world->addRigidBody(obj->bullet->body.get());
	}
}

// Adds a new object to physics world
void BumblebeeBullet::object_added(unsigned int id) {
	auto obj = _world->scene()->objects->at(id);
	if (obj->bullet) {
		if (obj->bullet->body) {
			obj->bullet->body->setUserIndex(id);
			_bullet_world->addRigidBody(obj->bullet->body.get());
		}
	}
}

// Removes an object from the physics world
void BumblebeeBullet::object_removed(unsigned int id) {
	auto obj = _world->scene()->objects->at(id);
	if (obj->bullet) {
		if (obj->bullet->body) {
			_bullet_world->removeRigidBody(obj->bullet->body.get());
		}
	}
}

bool BumblebeeBullet::init(BumblebeeWorld::ptr world) {
	_world = world;

	this->priority = 1;
	this->type = BUMBLEBEE_PHYSICS;

	_collision_config = new btDefaultCollisionConfiguration();
	_dispatcher = new btCollisionDispatcher(_collision_config);
	_broadphase = new btDbvtBroadphase();
	_solver = new btSequentialImpulseConstraintSolver();
	_bullet_world = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _collision_config);

	//_bullet_world->setGravity(btVector3(0, 0, 0));
	_bullet_world->setInternalTickCallback(simulation_tick, static_cast<void*>(this));
	_world->register_scene_changed_callback(std::bind(&BumblebeeBullet::scene_changed, this, std::placeholders::_1));

	return true;
}

void BumblebeeBullet::destroy() {
	for (auto i = _bullet_world->getNumCollisionObjects() - 1; i >= 0; i--)	{
		btCollisionObject* obj = _bullet_world->getCollisionObjectArray()[i];
		_bullet_world->removeCollisionObject(obj);	
	}

	delete _collision_config;
	delete _dispatcher;
	delete _broadphase;
	delete _solver;
	delete _bullet_world;

	_world.reset();
}