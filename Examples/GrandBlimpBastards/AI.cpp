#include "AI.h"
#include "Dependencies/include/bullet/btBulletDynamicsCommon.h"

void AI::tick() {
	spawn_timer -= _world->dt;
	fire_timer -= _world->dt;
	difficulty_setting += _world->dt;

	//spawn an enemy to chase you down and beat you down
	if (_world->scene()->objects->at(_world->scene()->player_id)->state == BumblebeeObject::ACTIVE) {

		glm::vec3 player_position = _world->scene()->objects->at(_world->scene()->player_id)->spatial->position;
		if (spawn_timer < 0.0f) {
			spawn_timer = spawn_setting - (difficulty_setting / 15.0f);
			if (spawn_timer < 0.5f) {
				spawn_timer = 0.5f;
			}

			std::random_device rd;
			std::mt19937 gen(rd());
			float min = player_position.x - 30.0f;
			float max = player_position.x + 30.0f;
			std::uniform_real_distribution<float> random_x(min, max);
			min = player_position.y - 5.0f;
			max = player_position.y + 5.0f;
			std::uniform_real_distribution<float> random_y(min, max);
			min = player_position.z + 40.0f;
			max = player_position.z + 50.0f;
			std::uniform_real_distribution<float> random_z(min, max);

			glm::vec3 random_spawn = glm::vec3(random_x(gen), random_y(gen), random_z(gen));

			//float angle = glm::atan(random_spawn.z + player_position.z, random_spawn.x - player_position.x);
			float angle = 0.0f;

			std::uniform_int_distribution<> half(0, 2);
			if (half(gen) == 1) {
				_world->scene()->objects->add(factory::create_blimp(random_spawn, _world->proxy->models["assets/models/capsule.ply"], _world->proxy->textures["assets/textures/red.dds"], angle, AIComponent::RAM));
			}
			else {
				_world->scene()->objects->add(factory::create_blimp(random_spawn, _world->proxy->models["assets/models/capsule.ply"], _world->proxy->textures["assets/textures/marble.dds"], angle));
			}
		}

		//do AI stuff for once
		for (unsigned int i = 0; i < _world->scene()->objects->size(); i++) {
			auto obj = _world->scene()->objects->at(i);
			if (obj && _world->scene()->objects->at(i)->ai->ai_group == AIComponent::ENEMY) {
				if (obj->state == BumblebeeObject::ACTIVE) {
					glm::vec3 enemy_position = _world->scene()->objects->at(i)->spatial->position;
					glm::vec3 slerp = player_position - enemy_position;

					float factor = 0.0f;
					switch (obj->ai->ai_weapon) {
						case AIComponent::GUN:
							if (fire_timer < 0.0f && glm::distance(enemy_position, player_position) <= 25.0f) {
								glm::vec3 cannon_dir = add_cannon_variance(player_position, 5.0f) - enemy_position;

								//enemy cannon position
								glm::vec3 cannon_origin;
								cannon_origin.x = enemy_position.x;
								cannon_origin.y = enemy_position.y - 2.0f;
								cannon_origin.z = enemy_position.z - 3.0f;

								auto cannonball_id = _world->scene()->objects->add(factory::create_sphere(cannon_origin, _world->proxy->models["assets/models/sphere.ply"], _world->proxy->textures["assets/textures/metal.dds"]));
								float modifier = 1.0f;
								auto impulse = btVector3(btScalar(cannon_dir.x * (modifier *1.15f)), btScalar((cannon_dir.y * modifier * 1.15f) + 5.0f), btScalar(cannon_dir.z * modifier));
								auto torque = btVector3(btScalar(20), btScalar(20), btScalar(0));
								_world->scene()->objects->at(cannonball_id)->model->info->shininess = 20.0f;
								_world->scene()->objects->at(cannonball_id)->bullet->body->applyCentralImpulse(impulse);
								_world->scene()->objects->at(cannonball_id)->bullet->body->applyTorque(torque);
							}

							if (glm::distance(enemy_position, player_position) >= 20.0f) {
								factor = 0.15f;
								_world->scene()->objects->at(i)->bullet->body->setLinearVelocity(btVector3(slerp.x * factor, slerp.y * factor, slerp.z * factor));
							}
							else {
								_world->scene()->objects->at(i)->bullet->body->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
							}

							break;

						case AIComponent::RAM:
							factor = 0.6f;
							_world->scene()->objects->at(i)->bullet->body->setLinearVelocity(btVector3(slerp.x * factor, slerp.y * factor, slerp.z * factor));
							break;
						default: break;
					}
				}
			}
		}
	}

	if (fire_timer < 0.0f) fire_timer = 2.0f;
}

glm::vec3 AI::add_cannon_variance(glm::vec3 position, float variance) {
	variance -= difficulty_setting / 80.0f;
	if (variance <= 2.0f) variance = 2.0f;

	std::random_device rd;
	std::mt19937 gen(rd());
	float min = position.x - variance;
	float max = position.x + variance;
	std::uniform_real_distribution<float> random_x(min, max);
	min = position.y - variance;
	max = position.y + variance;
	std::uniform_real_distribution<float> random_y(min, max);
	min = position.z + variance;
	max = position.z + variance;
	std::uniform_real_distribution<float> random_z(min, max);

	glm::vec3 estimated_pos = glm::vec3(random_x(gen), random_y(gen), random_z(gen));

	return estimated_pos;
}

bool AI::init(BumblebeeWorld::ptr world) {
	_world = world;
	this->difficulty_setting = 0.0f;
	this->spawn_setting = 6.0f;
	this->spawn_timer = 1.0f;
	this->fire_timer = 1.5f;
	return true;
}

// Destroys system
void AI::destroy() {
	_world.reset();
}