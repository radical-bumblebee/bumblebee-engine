#ifndef _BlimpFactory_H_
#define _BlimpFactory_H_

#include <BumblebeeObject.h>

namespace factory {
	static BumblebeeLight::ptr create_light(LightType type, glm::vec3 position) {
		BumblebeeLight::ptr light = std::make_shared<BumblebeeLight>();

		light->position = glm::vec4(position, 1.0f);
		light->intensities = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		light->ambient = 0.08f;
		light->attenuation = 0.1f;

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
			light->intensities = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			light->position.w = 0.0f;
			break;
		}

		return light;
	}

	static BumblebeeObject::ptr create_soft(ModelInfo::ptr info) {
		BumblebeeObject::ptr obj = std::make_shared<BumblebeeObject>();
		obj->spatial = std::make_shared<SpatialComponent>(glm::vec3(0.0f), glm::vec3(1.0f));
		obj->ai = std::make_shared<AIComponent>(AIComponent::NONE);
		obj->model = std::make_shared<ModelComponent>(info, 0);
		obj->model->info->shininess = 1.0f;
		obj->model->info->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		obj->model->info->update_mesh = true;
		obj->bullet = nullptr;
		obj->state = BumblebeeObject::UNUSED;

		return obj;
	}

	static BumblebeeObject::ptr create_blimp(glm::vec3 position, ModelInfo::ptr info, unsigned int texture, float rotate = 0.0f, AIComponent::AIWeapon weapon = AIComponent::GUN) {
		BumblebeeObject::ptr obj = std::make_shared<BumblebeeObject>();
		obj->spatial = std::make_shared<SpatialComponent>(position, glm::vec3(1.0f));
		obj->spatial->orientation = glm::rotate(obj->spatial->orientation, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		obj->spatial->orientation = glm::rotate(obj->spatial->orientation, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
		obj->model = std::make_shared<ModelComponent>(info, texture);
		obj->model->info->shininess = 1.0f;

		btTransform transform;
		obj->bullet->mass = 10.0f;
		obj->bullet->shape = new btCapsuleShape(btScalar(obj->spatial->scale.x), btScalar(obj->spatial->scale.y * 2.5f));
		obj->bullet->shape->calculateLocalInertia(obj->bullet->mass, obj->bullet->local_inertia);
		transform.setIdentity();
		transform.setOrigin(btVector3(obj->spatial->position.x, obj->spatial->position.y, obj->spatial->position.z));
		transform.setRotation(btQuaternion(btScalar(obj->spatial->orientation.x), btScalar(obj->spatial->orientation.y), btScalar(obj->spatial->orientation.z), btScalar(obj->spatial->orientation.w)));
		obj->bullet->motion_state = std::make_shared<btDefaultMotionState>(transform);
		btRigidBody::btRigidBodyConstructionInfo rigid_info(obj->bullet->mass, obj->bullet->motion_state.get(), obj->bullet->shape, obj->bullet->local_inertia);
		rigid_info.m_friction = 1.0f;
		rigid_info.m_restitution = 1.0f;
		obj->bullet->body = std::shared_ptr<btRigidBody>(new btRigidBody(rigid_info));
		obj->bullet->body->setFlags(BT_DISABLE_WORLD_GRAVITY);
		obj->ai = std::make_shared<AIComponent>(AIComponent::ENEMY);
		obj->ai->ai_weapon = weapon;
		obj->state = BumblebeeObject::ACTIVE;

		return obj;
	}

	static BumblebeeObject::ptr create_sphere(glm::vec3 position, ModelInfo::ptr info, unsigned int texture) {

		BumblebeeObject::ptr obj = std::make_shared<BumblebeeObject>();
		obj->spatial = std::make_shared<SpatialComponent>(position, glm::vec3(0.5f));
		obj->model = std::make_shared<ModelComponent>(info, texture);
		obj->model->info->shininess = 1.0f;
		obj->ai = std::make_shared<AIComponent>(AIComponent::NONE);

		btTransform transform;
		obj->bullet->mass = 1.0f;
		obj->bullet->shape = new btSphereShape(btScalar(obj->spatial->scale.x));
		obj->bullet->shape->calculateLocalInertia(obj->bullet->mass, obj->bullet->local_inertia);
		transform.setIdentity();
		transform.setOrigin(btVector3(obj->spatial->position.x, obj->spatial->position.y, obj->spatial->position.z));
		obj->bullet->motion_state = std::make_shared<btDefaultMotionState>(transform);
		btRigidBody::btRigidBodyConstructionInfo rigid_info(obj->bullet->mass, obj->bullet->motion_state.get(), obj->bullet->shape, obj->bullet->local_inertia);
		rigid_info.m_friction = 1.0f;
		rigid_info.m_restitution = 0.6f;
		obj->bullet->body = std::shared_ptr<btRigidBody>(new btRigidBody(rigid_info));
		obj->state = BumblebeeObject::UNUSED;

		return obj;
	}

	static BumblebeeObject::ptr create_plane(glm::vec3 position, ModelInfo::ptr info, unsigned int texture, float rotate = 0.0f, glm::vec3 axis = glm::vec3(0.0f, 0.0f, 1.0f)) {
		BumblebeeObject::ptr obj = std::make_shared<BumblebeeObject>();
		obj->spatial = std::make_shared<SpatialComponent>(position, glm::vec3(2.0f));
		//obj->spatial->orientation = glm::rotate(obj->spatial->orientation, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		obj->spatial->orientation = glm::rotate(obj->spatial->orientation, glm::radians(rotate), axis);
		obj->model = std::make_shared<ModelComponent>(info, texture);
		obj->model->info->shininess = 1.0f;
		obj->bullet = nullptr;

		return obj;
	}

	static BumblebeeObject::ptr create_skybox(ModelInfo::ptr info, unsigned int texture) {
		BumblebeeObject::ptr obj = std::make_shared<BumblebeeObject>();
		obj->ai = std::make_shared<AIComponent>(AIComponent::NONE);
		obj->spatial = std::make_shared<SpatialComponent>(glm::vec3(0), glm::vec3(100.0f));
		obj->model = std::make_shared<ModelComponent>(info, texture);
		obj->model->info->shininess = 1.0f;
		obj->bullet = nullptr;
		obj->state = BumblebeeObject::UNUSED;

		return obj;
	}
}

#endif