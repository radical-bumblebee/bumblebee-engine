#include "BumblebeeInput.h"

void BumblebeeInput::tick() {
	//do some mouse shit first
	if (_scene_info->scene_id == 0) {
		int x = 0;
		int y = 0;
		SDL_GetMouseState(&x, &y);
		float fraction = 0.0f;
		float half_width = this->win_x / 2.0f;
		if (x < half_width) {
			fraction = (half_width - x) / half_width;
			_world->scene()->objects->at(_world->scene()->player_id)->bullet->body->applyCentralImpulse(btVector3(btScalar(0.0), btScalar(0.0), btScalar(0.4 * fraction)));
		}
		else if (x >= half_width) {
			fraction = (x - half_width) / half_width;
			_world->scene()->objects->at(_world->scene()->player_id)->bullet->body->applyCentralImpulse(btVector3(btScalar(0.0), btScalar(0.0), btScalar(-0.4 * fraction)));
		}
	}

	for (int idx = 0; idx < 256; idx++)	{
		if (tapped_keys->pressed(idx)) {
			switch (idx) {
				case SDLK_SPACE:
				{
					//btVector3 vel = _world->scene()->objects->at(_world->scene()->player_id)->bullet->body->getLinearVelocity();
					//_world->scene()->objects->at(_world->scene()->player_id)->bullet->body->setLinearVelocity(btVector3(vel.getX(), 0.0f, vel.getZ()));
					_world->scene()->objects->at(_world->scene()->player_id)->bullet->body->applyCentralImpulse(btVector3(btScalar(0.0), btScalar(15.0), btScalar(0.0)));
					tapped_keys->set_pressed(idx, false);
					break;
				}
			}
		}
	}

	for (int idx = 0; idx < 256; idx++)	{
		if (held_keys->pressed(idx)) {
			switch (idx) {
			case SDLK_DOWN - 0x40000000:
			{
				_world->scene()->objects->at(_world->scene()->player_id)->bullet->body->applyCentralImpulse(btVector3(btScalar(0.2), btScalar(0.0), btScalar(0.0)));
				break;
			}
			case SDLK_UP - 0x40000000:
			{
				_world->scene()->objects->at(_world->scene()->player_id)->bullet->body->applyCentralImpulse(btVector3(btScalar(-0.2), btScalar(0.0), btScalar(0.0)));

				break;
			}
			case SDLK_LEFT - 0x40000000:
			{
				_world->scene()->objects->at(_world->scene()->player_id)->bullet->body->applyCentralImpulse(btVector3(btScalar(0.0), btScalar(0.0), btScalar(0.2)));

				break;
			}
			case SDLK_RIGHT - 0x40000000:
			{
				_world->scene()->objects->at(_world->scene()->player_id)->bullet->body->applyCentralImpulse(btVector3(btScalar(0.0), btScalar(0.0), btScalar(-0.2)));

				break;
			}

			case SDLK_m:
			{
				_world->proxy->fonts["assets/fonts/times.ttf"]->set_font_size(48);
				break;
			}

			case SDLK_w:
			{
				_world->scene()->camera()->eye.y += 0.1f;
				break;
			}
			case SDLK_a:
			{
				if (_scene_info->scene_id == 0) {
					_world->scene()->objects->at(_world->scene()->player_id)->bullet->body->applyCentralImpulse(btVector3(btScalar(0.0), btScalar(0.0), btScalar(0.2)));
				}
				else {
					_world->scene()->camera()->eye.x += 0.1f;
				}
				break;
			}
			case SDLK_s:
			{
				_world->scene()->camera()->eye.y -= 0.1f;
				break;
			}
			case SDLK_d:
			{
				if (_scene_info->scene_id == 0) {
					_world->scene()->objects->at(_world->scene()->player_id)->bullet->body->applyCentralImpulse(btVector3(btScalar(0.0), btScalar(0.0), btScalar(-0.2)));
				}
				else {
					_world->scene()->camera()->eye.x -= 0.1f;
				}
				break;
			}

			case SDLK_o:
			{
				BumblebeeObject::ptr obj = std::make_shared<BumblebeeObject>();

				obj->model->info = _world->proxy->models["assets/models/sphere.ply"];
				obj->model->texture = _world->proxy->textures["assets/textures/marble.dds"];
				obj->model->info->color = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
				obj->model->info->shininess = 1.0f;

				obj->spatial->position = glm::vec3(6.0f, 2.0f, 0.0f);
				obj->spatial->scale = glm::vec3(1.0f, 1.0f, 1.0f);

				btTransform transform;
				obj->bullet->mass = 2.0f;
				obj->bullet->shape = new btSphereShape(obj->spatial->scale.x);
				//obj->bullet->shape = std::make_shared<btSphereShape>(btScalar(obj->spatial->scale.x));
				obj->bullet->shape->calculateLocalInertia(obj->bullet->mass, obj->bullet->local_inertia);
				transform.setIdentity();
				transform.setOrigin(btVector3(obj->spatial->position.x, obj->spatial->position.y, obj->spatial->position.z));
				obj->bullet->motion_state = std::make_shared<btDefaultMotionState>(transform);
				btRigidBody::btRigidBodyConstructionInfo rigid_info(obj->bullet->mass, obj->bullet->motion_state.get(), obj->bullet->shape, obj->bullet->local_inertia);
				rigid_info.m_friction = 1.0f;
				rigid_info.m_restitution = 0.6f;
				obj->bullet->body = //std::make_shared<btRigidBody>(rigid_info);
					std::shared_ptr<btRigidBody>(new btRigidBody(rigid_info));
				_world->scene()->objects->add(obj);
				break;
			}
			case SDLK_u:
			{
				auto scores = BumblebeePersist::get()->get_top_20();
				int i = 0;
				for (auto score : scores) {
					++i;
					std::string score_string = "ID: " + std::to_string(score.first) + "     Score: " + std::to_string(score.second);
					_world->scene()->ui->add(std::make_shared<BumblebeeUIElement>(score_string.c_str(), "assets/fonts/times.ttf", 10.0f, (30.0f*i) + 50.0f, &glm::vec4(1.0f)));
				}
				break;
			}
			case SDLK_1:
			{
				//BumblebeePersist::get()->persist_to_json(&_scene->objects.at(0), "assets/json/beejson.json");
				//_world->scene()->objects->at(_world->scene()->player_id)->model->info = _world->proxy->model["assets/models/cylinder.ply"];
				_scene_info->scene_id = 0;
				_world->activate_scene(0);
				break;
			}
			case SDLK_2:
			{
				_scene_info->scene_id = 1;
				_world->activate_scene(1);
				break;
			}
			case SDLK_r:
			{
				break;
			}
			}
		}
	}
}

void BumblebeeInput::set_key(unsigned char key, bool pressed_value) {
	held_keys->keys[key] = pressed_value;
}

void BumblebeeInput::set_mouse_xy(int x, int y) {
	mouse.x = x;
	mouse.y = y;
}

BumblebeeInput::BeeKeys::BeeKeys() {
	keys = new bool[256];
	for (int idx = 0; idx < 256; idx++)
	{
		keys[idx] = false;
	}
	up = false;
	down = false;
	left = false;
	right = false;
}

// Checks if a key is pressed
bool BumblebeeInput::BeeKeys::pressed(unsigned char key) {
	return keys[key];
}

// Sets a pressed key
void BumblebeeInput::BeeKeys::set_pressed(unsigned char key, bool value) {
	if (key > 500) {
		key -= (unsigned char)0x40000000;
	}

	keys[key] = value;
}

bool BumblebeeInput::init(BumblebeeWorld::ptr world) {
	_world = world;

	this->held_keys = std::make_shared<BumblebeeInput::BeeKeys>();
	this->tapped_keys = std::make_shared<BumblebeeInput::BeeKeys>();

	return true;
}

void BumblebeeInput::destroy() {
	_world.reset();
}

void BumblebeeInput::key_click(SDL_KeyboardEvent e) {
	if (e.type == SDL_KEYDOWN) {
		if (e.repeat == 0) {
			this->tapped_keys->set_pressed(e.keysym.sym, true);
			this->held_keys->set_pressed(e.keysym.sym, true);
		}
		else {
			this->held_keys->set_pressed(e.keysym.sym, true);
		}
	}

	if (e.type == SDL_KEYUP) {
		this->held_keys->set_pressed(e.keysym.sym, false);
	}
}

void BumblebeeInput::mouse_event(SDL_Event e, int winx, int winy) {
	if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
		//btVector3 vel = _world->scene()->objects->at(_world->scene()->player_id)->bullet->body->getLinearVelocity();
		//_world->scene()->objects->at(_world->scene()->player_id)->bullet->body->setLinearVelocity(btVector3(vel.getX(), 0.0f, vel.getZ()));
		_world->scene()->objects->at(_world->scene()->player_id)->bullet->body->applyCentralImpulse(btVector3(btScalar(0.0), btScalar(15.0), btScalar(0.0)));
	}
	else if (e.type == SDL_MOUSEMOTION) {
		this->win_x = winx;
		this->win_y = winy;
	}
}