#include "SceneDirector_old.h"

void SceneDirector::tick() {
	if (_scene_info->scene_id == 0) {
		auto player = _world->scene()->objects->at(_world->scene()->player_id);
		auto velocity = player->bullet->body->getLinearVelocity();
		velocity.setX(-5.0);
		player->bullet->body->setLinearVelocity(velocity);

		btTransform transform;
		player->bullet->body->getMotionState()->getWorldTransform(transform);

		auto obj_pos = glm::vec3(transform.getOrigin().x(), transform.getOrigin().y(), transform.getOrigin().z());

		_world->scene()->lights->at(0)->position = glm::vec4(obj_pos.x, obj_pos.y, obj_pos.z, 1.0f);

		if (((_segment_end + obj_pos.x) < SEGMENT_LENGTH * 7.0f)) {
			generate_segment();
		}

		if (!_pillars.empty()) {
			if (obj_pos.x < _pillars.front()->spatial->position.x) {
				_pillars.front().reset();
				_pillars.pop();
				score++;
				_world->scene()->ui->at(0)->text = "Current Score: " + std::to_string(score);
			}
		}

		_world->scene()->camera()->third_person_view(_world->scene()->objects->at(_world->scene()->player_id)->spatial->position);

		//_world->scene()->ui->at(0)->text = "FPS: " + std::to_string((int)std::ceil(_world->fps));
	}
}

void SceneDirector::generate_segment(unsigned int num_segments, bool generate_pillars) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> half(0, 1);
	std::uniform_int_distribution<> third(0, 2);

	for (auto i = _segment_index; i < (_segment_index + num_segments); ++i) {
		_world->scene()->objects->add(_factory->create_plane(i * (-PLANE_LENGTH * 2), 0.0f, 0.0f));
		_world->scene()->objects->add(_factory->create_plane(i * (-PLANE_LENGTH * 2), PLANE_LENGTH, PLANE_LENGTH, 270.0f));
		_world->scene()->objects->add(_factory->create_plane(i * (-PLANE_LENGTH * 2), PLANE_LENGTH * 2, 0.0f, 180.0f));
		_world->scene()->objects->add(_factory->create_plane(i * (-PLANE_LENGTH * 2), PLANE_LENGTH, -PLANE_LENGTH, 90.0f));

		if (generate_pillars) {
			for (auto j = 1; j > -2; --j) {
				if (half(gen) == 0) {
					auto pillar_x = (i * -SEGMENT_LENGTH) + (j * PILLAR_LENGTH);
					auto pillar_y = ((third(gen) * 0.7f) - 0.7f) + 1.0f;
					auto pillar_z = (third(gen) * PILLAR_LENGTH) - PILLAR_LENGTH;

					BumblebeeObject::ptr obj = nullptr;

					if (half(gen) == 0) {
						obj = _factory->create_cylinder(pillar_x, pillar_y, 0.0f, 90.0f);
					}
					else {
						obj = _factory->create_cylinder(pillar_x, 1.0f, pillar_z);
					}

					_world->scene()->objects->add(obj);
					_pillars.push(obj);
				}
			}
		}
		_segment_end += SEGMENT_LENGTH;
	}
	_segment_index += num_segments;
}

bool SceneDirector::init(BumblebeeWorld::ptr world) {
	_world = world;

	_segment_begin = 0.0f;
	_segment_end = _segment_begin;

	return true;
}

void SceneDirector::destroy() {
	_world.reset();
}