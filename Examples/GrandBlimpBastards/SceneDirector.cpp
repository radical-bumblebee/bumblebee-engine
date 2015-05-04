#include "SceneDirector.h"
#include "SceneInfo.h"
#include <sstream>

void SceneDirector::tick() {
	if (_world->config->game_over < 1.0f && _world->config->game_over > 0.0f) {
		_world->config->game_over -= 0.3f * _world->dt;
		if (_world->config->game_over < 0.0f) {
			_world->config->game_over = 0.0f;
			_world->scene()->ui->add(std::make_shared<BumblebeeUIElement>("Game Over", "assets/fonts/times.ttf", _world->window_w / 2.0f - 35.0f, _world->window_h / 2.0f - 50.0f, &glm::vec4(1.0f)));
			_world->scene()->ui->add(std::make_shared<BumblebeeUIElement>("Hit R to retry", "assets/fonts/times.ttf", _world->window_w / 2.0f - 40.0f, _world->window_h / 2.0f - 20.0f, &glm::vec4(1.0f)));
		}
	}

	if (_world->config->game_over > 0.0f) {
		difficulty += _world->dt;

		//clean up destroyed blimps by setting them to unused
		for (unsigned int i = 0; i < _world->scene()->objects->size(); i++) {
			switch (_world->scene()->objects->at(i)->state) {
			case BumblebeeObject::INACTIVE:
				if (_world->scene()->objects->at(i)->model->info->name == _scene_info->blimp_name) {
					_world->scene()->objects->at(i)->state = BumblebeeObject::UNUSED;
					high_score++;
				}

				break;
			}
		}

		_scene_info->scene_timer -= _world->dt;

		std::string high_score_string = "Your Score: ";
		high_score_string += std::to_string(high_score);
		_world->scene()->ui->at(0)->text = high_score_string;

		if (_scene_info->rotate) {
			_world->scene()->camera()->rotate_radians(rotation);
			rotation += 0.2f;
			if (rotation > 360.0f) {
				rotation = 0.0f;
			}
		}

		// Color the skies as time goes on
		if (_world->scene()->objects->at(_scene_info->skybox_id)->model->info->transparency < 3.0f) {
			_world->scene()->objects->at(_scene_info->skybox_id)->model->info->transparency += (difficulty / 2000.0f) * _world->dt;
		}
		else {
			if (_world->scene()->objects->at(_scene_info->skybox_id)->model->info->shininess > 0.2f) {
				_world->scene()->objects->at(_scene_info->skybox_id)->model->info->shininess -= (difficulty / 3000.0f) * _world->dt;
			}
		}

		// Updates player blimp location
		_scene_info->cannon_origin.x = _world->scene()->objects->at(_world->scene()->player_id)->spatial->position.x;
		_scene_info->cannon_origin.y = _world->scene()->objects->at(_world->scene()->player_id)->spatial->position.y - 2.0f;
		_scene_info->cannon_origin.z = _world->scene()->objects->at(_world->scene()->player_id)->spatial->position.z + 3.0f;

		auto data_ptr = _world->scene()->objects->at(_scene_info->line_id)->model->info->vertices.data();
		data_ptr->x = _scene_info->cannon_origin.x;
		data_ptr->y = _scene_info->cannon_origin.y;
		data_ptr->z = _scene_info->cannon_origin.z;
		data_ptr++;

		int x = 0; int y = 0;
		SDL_GetMouseState(&x, &y);

		// Transforms mouse position to 3D space
		float depth = 0.0f;
		glReadPixels((GLint)x, (GLint)(_world->window_h - y - 1.0f), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		depth = glm::clamp(depth, 0.8f, 0.95f);
		glm::vec4 viewport = glm::vec4(0.0f, 0.0f, _world->window_w, _world->window_h);
		glm::vec3 window_coordinates = glm::vec3(x, _world->window_h - y - 1.0f, depth);
		_scene_info->target_direction = glm::unProject(window_coordinates, _world->scene()->camera()->view_matrix, _world->projection_matrix, viewport);

		// Updates player blimp targetting system
		data_ptr->x = _scene_info->target_direction.x;
		data_ptr->y = _scene_info->target_direction.y;
		data_ptr->z = _scene_info->target_direction.z;
	}
}

// Spawns a cannonball and fires it towards mouse position
void SceneDirector::fire_cannon() {
	if (_scene_info->scene_timer < 0.0f) {
		auto cannonball_id = _world->scene()->objects->add(factory::create_sphere(_scene_info->cannon_origin, _world->proxy->models["assets/models/sphere.ply"], _world->proxy->textures["assets/textures/metal.dds"]));
		float modifier = 2.0f;
		auto impulse = btVector3(btScalar(_scene_info->target_direction.x * (modifier * 1.15f)), btScalar((_scene_info->target_direction.y * modifier * 1.15f) + 5.0f), btScalar(_scene_info->target_direction.z * modifier));
		auto torque = btVector3(btScalar(20), btScalar(20), btScalar(0));
		_world->scene()->objects->at(cannonball_id)->model->info->shininess = 20.0f;
		_world->scene()->objects->at(cannonball_id)->bullet->body->applyCentralImpulse(impulse);
		_world->scene()->objects->at(cannonball_id)->bullet->body->applyTorque(torque);
		_scene_info->scene_timer = RESET_PERIOD;
	}
}

void SceneDirector::blimp_collision(BumblebeeObject* obj, glm::vec3 contact_point) {
	obj->bullet->body->setFlags(obj->bullet->body->getFlags() & ~BT_DISABLE_WORLD_GRAVITY);

	if (obj->model->info->name == _scene_info->blimp_name) {
		obj->particle = std::make_shared<ParticleComponent>();
		obj->particle->info = _world->proxy->particles["assets/textures/smoke.dds"];
		obj->particle->particle_offset = glm::vec4(obj->spatial->position - (contact_point), 1.0f);			

		if (obj->state == BumblebeeObject::ACTIVE)
			obj->state = BumblebeeObject::INACTIVE;

	}
}

void SceneDirector::collision_callback(unsigned int id_a, unsigned int id_b, glm::vec3 contact_point) {
	auto obj_a = _world->scene()->objects->at(id_a);
	auto obj_b = _world->scene()->objects->at(id_b);

	if (_world->scene()->player_id == id_a || _world->scene()->player_id == id_b) {
		_world->config->game_over -= 0.0000001f;
		if (high_score > (unsigned int)_scene_info->highest_score) {
			BumblebeePersist::get()->set_high_score(0, high_score);
		}
	}
	
	// Drops blimps from the sky upon impact
	blimp_collision(obj_a, contact_point);
	blimp_collision(obj_b, contact_point);
}

void SceneDirector::init_scene() {
	// Initialize scene
	_scene_info->scene_id = _world->create_scene();
	_world->activate_scene(_scene_info->scene_id);
	_world->scene()->camera()->eye = glm::vec3(0.0f, 2.0f, 0.0f);
	_world->scene()->camera()->target = glm::vec3(0.0f, 0.0f, 5.0f);

	// Create player blimp
	auto obj = factory::create_blimp(glm::vec3(0.0f), _world->proxy->models["assets/models/capsule.ply"], _world->proxy->textures["assets/textures/marble.dds"]);
	_world->scene()->player_id = _world->scene()->objects->add(obj);
	obj->ai->ai_group = AIComponent::PLAYER;

	_scene_info->line_id = _world->scene()->objects->add(factory::create_soft(_world->proxy->models["line"]));
	_scene_info->skybox_id = _world->scene()->objects->add(factory::create_skybox(_world->proxy->models["assets/models/cube.ply"], _world->proxy->textures["skybox"]));

	// Create lights
	_world->scene()->lights->add(factory::create_light(DIRECTIONAL_LIGHT, glm::vec3(0.0f, 10.0f, 0.0f)));
	_world->scene()->lights->add(factory::create_light(POINT_LIGHT, glm::vec3(0.0f, 5.0f, 20.0f)));

	// Create ui
	_world->scene()->ui->add(std::make_shared<BumblebeeUIElement>("Your Score: ", "assets/fonts/times.ttf", 10.0f, _world->window_h - 30.0f, &glm::vec4(1.0f)));

	_scene_info->highest_score = BumblebeePersist::get()->get_high_score(0);
	std::string highest_score_string;
	if (_scene_info->highest_score > 0) {
		highest_score_string = std::to_string(_scene_info->highest_score);
	}
	else {
		highest_score_string = "-";
	}
	_world->scene()->ui->add(std::make_shared<BumblebeeUIElement>("High Score: " + highest_score_string, "assets/fonts/times.ttf", 10.0f, _world->window_h - 55.0f, &glm::vec4(1.0f)));
}

bool SceneDirector::init(BumblebeeWorld::ptr world) {
	_world = world;
	rotation = 0.0f;
	high_score = 0;
	difficulty = 0.0f;

	return true;
}

void SceneDirector::destroy() {
	_world.reset();
}