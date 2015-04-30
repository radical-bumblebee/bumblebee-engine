#include "BumblebeeObject.h"

BumblebeeObject::BumblebeeObject(const BumblebeeObject& rhs) {
	this->spatial = rhs.spatial;
	this->model = rhs.model;
	this->bullet = rhs.bullet;
	this->particle = rhs.particle;
	this->ai = rhs.ai;
	this->state = ACTIVE;
}

BumblebeeObject& BumblebeeObject::operator=(const BumblebeeObject& rhs) {
	this->spatial = rhs.spatial;
	this->model = rhs.model;
	this->bullet = rhs.bullet;
	this->particle = rhs.particle;
	this->ai = rhs.ai;
	this->state = ACTIVE;
	return *this;
}

BumblebeeObject::BumblebeeObject(BumblebeeObject &&rhs) {
	if (this != &rhs) {
		this->spatial = std::move(rhs.spatial);
		this->model = std::move(rhs.model);
		this->bullet = std::move(rhs.bullet);
		this->particle = std::move(rhs.particle);
		this->ai = std::move(rhs.ai);
		this->state = std::move(rhs.state);
	}
};

BumblebeeObject& BumblebeeObject::operator=(BumblebeeObject &&rhs) {
	if (this != &rhs) {
		this->spatial = std::move(rhs.spatial);
		this->model = std::move(rhs.model);
		this->bullet = std::move(rhs.bullet);
		this->particle = std::move(rhs.particle);
		this->ai = std::move(rhs.ai);
		this->state = std::move(rhs.state);
	}
	return *this;
};

BumblebeeObject::BumblebeeObject() {
	spatial = std::make_shared<SpatialComponent>();
	model = std::make_shared<ModelComponent>();
	bullet = std::make_shared<BulletComponent>();
	particle = nullptr;
	ai = std::make_shared<AIComponent>();
	state = ACTIVE;
}

BumblebeeObject::~BumblebeeObject() {
	particle.reset();
	bullet.reset();
	model.reset();
	spatial.reset();
	ai.reset();
}