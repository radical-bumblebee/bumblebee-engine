#include "DemoFactory.h"

BumblebeeLight::ptr DemoFactory::create_light(LightType type, float x, float y, float z) {
	BumblebeeLight::ptr light = std::make_shared<BumblebeeLight>();

	light->position = glm::vec4(x, y, z, 1.0f);
	light->intensities = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	light->ambient = 0.0f;
	light->attenuation = 0.01f;

	switch (type) {
	case POINT_LIGHT:
		light->cone_angle = 0.0f;
		break;
	case SPOT_LIGHT:
		light->cone_angle = 90.0f;
		light->intensities = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
		light->cone_direction = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
		break;
	case DIRECTIONAL_LIGHT:
		light->intensities = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
		light->position.w = 0.0f;
		break;
	}

	return light;
}

// Creates and adds a plane object to scene
BumblebeeObject::ptr DemoFactory::create_plane(float x, float y, float z, float angle) {
	BumblebeeObject::ptr obj = std::make_shared<BumblebeeObject>();

	obj->model->info = _world->proxy->models["assets/models/plane.ply"];
	obj->model->texture = _world->proxy->textures["assets/textures/cobblestone.dds"];
	obj->model->info->color = glm::vec4(0.5f, 1.0f, 0.2f, 1.0f);
	obj->model->info->shininess = 1.0f;

	obj->spatial->position = glm::vec3(x, y, z);
	obj->spatial->scale = glm::vec3(7.5f, 0.1f, 7.5f);
	obj->spatial->orientation = glm::rotate(obj->spatial->orientation, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));

	btTransform transform;
	transform.setIdentity();
	transform.setRotation(btQuaternion(btScalar(obj->spatial->orientation.x), btScalar(obj->spatial->orientation.y), btScalar(obj->spatial->orientation.z), btScalar(obj->spatial->orientation.w)));
	transform.setOrigin(btVector3(btScalar(obj->spatial->position.x), btScalar(obj->spatial->position.y), btScalar(obj->spatial->position.z)));
	obj->bullet->mass = btScalar(0.0);
	obj->bullet->shape = new btBoxShape(btVector3(btScalar(obj->spatial->scale.x), btScalar(obj->spatial->scale.y), btScalar(obj->spatial->scale.z)));
	obj->bullet->shape->calculateLocalInertia(obj->bullet->mass, obj->bullet->local_inertia);
	obj->bullet->motion_state = std::make_shared<btDefaultMotionState>(transform);
	btRigidBody::btRigidBodyConstructionInfo rigid_info(obj->bullet->mass, obj->bullet->motion_state.get(), obj->bullet->shape, obj->bullet->local_inertia);
	rigid_info.m_friction = 0.5f;
	rigid_info.m_restitution = 1.0f;
	obj->bullet->body = std::shared_ptr<btRigidBody>(new btRigidBody(rigid_info));
	obj->bullet->body->setFriction(10.0f);

	return obj;
}

// Creates and adds a sphere object to the scene
BumblebeeObject::ptr DemoFactory::create_object(float x, float y, float z) {
	BumblebeeObject::ptr obj = std::make_shared<BumblebeeObject>();

	obj->model->info = _world->proxy->models["assets/models/sphere.ply"];
	obj->model->texture = _world->proxy->textures["assets/textures/marble.dds"];
	obj->model->info->color = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
	obj->model->info->shininess = 1.0f;

	obj->spatial->position = glm::vec3(x, y, z);
	obj->spatial->scale = glm::vec3(1.0f, 1.0f, 1.0f);

	btTransform transform;
	obj->bullet->mass = 2.0f;
	obj->bullet->shape = new btSphereShape(btScalar(obj->spatial->scale.x));
	obj->bullet->shape->calculateLocalInertia(obj->bullet->mass, obj->bullet->local_inertia);
	transform.setIdentity();
	transform.setOrigin(btVector3(obj->spatial->position.x, obj->spatial->position.y, obj->spatial->position.z));
	obj->bullet->motion_state = std::make_shared<btDefaultMotionState>(transform);
	btRigidBody::btRigidBodyConstructionInfo rigid_info(obj->bullet->mass, obj->bullet->motion_state.get(), obj->bullet->shape, obj->bullet->local_inertia);
	rigid_info.m_friction = 1.0f;
	rigid_info.m_restitution = 0.6f;
	obj->bullet->body = std::shared_ptr<btRigidBody>(new btRigidBody(rigid_info));
	obj->bullet->body->setAnisotropicFriction(obj->bullet->shape->getAnisotropicRollingFrictionDirection(), btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
	obj->bullet->body->forceActivationState(DISABLE_DEACTIVATION);
	//obj->bullet->shape->setUserPointer(obj->bullet.get());

	return obj;
}

BumblebeeObject::ptr DemoFactory::create_cylinder(float x, float y, float z, float angle) {
	//return create_cube(x, y, z);
	BumblebeeObject::ptr obj = std::make_shared<BumblebeeObject>();

	obj->model->info = _world->proxy->models["assets/models/cylinder.ply"];
	obj->model->texture = _world->proxy->textures["assets/textures/brick.dds"];
	obj->model->info->color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	obj->model->info->shininess = 0.8f;

	obj->spatial->position = glm::vec3(x, y, z);
	obj->spatial->scale = glm::vec3(2.0f, 7.5f, 2.0f);
	obj->spatial->orientation = glm::rotate(obj->spatial->orientation, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));

	btTransform transform;
	obj->bullet->mass = 0.0f;

	obj->bullet->shape = new btCylinderShape(btVector3(btScalar(obj->spatial->scale.x), btScalar(obj->spatial->scale.y), btScalar(obj->spatial->scale.z)));
	obj->bullet->shape->calculateLocalInertia(obj->bullet->mass, obj->bullet->local_inertia);

	transform.setIdentity();
	transform.setOrigin(btVector3(obj->spatial->position.x, obj->spatial->position.y * (obj->spatial->scale.y), obj->spatial->position.z));
	transform.setRotation(btQuaternion(btScalar(obj->spatial->orientation.x), btScalar(obj->spatial->orientation.y), btScalar(obj->spatial->orientation.z), btScalar(obj->spatial->orientation.w)));
	obj->bullet->motion_state = std::make_shared<btDefaultMotionState>(transform);
	btRigidBody::btRigidBodyConstructionInfo rigid_info(obj->bullet->mass, obj->bullet->motion_state.get(), obj->bullet->shape, obj->bullet->local_inertia);
	rigid_info.m_friction = 10.0f;
	rigid_info.m_restitution = 0.0f;
	obj->bullet->body = std::shared_ptr<btRigidBody>(new btRigidBody(rigid_info));

	return obj;
}

BumblebeeObject::ptr DemoFactory::create_cube(float x, float y, float z) {
	BumblebeeObject::ptr obj = std::make_shared<BumblebeeObject>();

	obj->spatial->position = glm::vec3(x, y, z);
	obj->spatial->scale = glm::vec3(1.0f, 1.0f, 1.0f);

	obj->model->info = _world->proxy->models["assets/models/cube.ply"];
	obj->model->info->color = glm::vec4(0.2f, 0.2f, 1.0f, 1.0f);
	obj->model->info->shininess = 1.0f;

	btTransform transform;
	obj->bullet->mass = 1.0f;
	obj->bullet->shape = new btBoxShape(btVector3(btScalar(1), btScalar(1), btScalar(1)));
	obj->bullet->shape->calculateLocalInertia(obj->bullet->mass, obj->bullet->local_inertia);
	transform.setIdentity();
	transform.setOrigin(btVector3(obj->spatial->position.x, obj->spatial->position.y, obj->spatial->position.z));
	obj->bullet->motion_state = std::make_shared<btDefaultMotionState>(transform);
	btRigidBody::btRigidBodyConstructionInfo rigid_info(obj->bullet->mass, obj->bullet->motion_state.get(), obj->bullet->shape, obj->bullet->local_inertia);
	rigid_info.m_friction = 0.1f;
	rigid_info.m_restitution = 0.2f;
	obj->bullet->body = std::shared_ptr<btRigidBody>(new btRigidBody(rigid_info));

	return obj;
}