#ifndef _SceneDirector_H_
#define _SceneDirector_H_

#include <ISystem.h>
#include <Dependencies/include/SDL/SDL.h>
#include <BumblebeePersist.h>

#include "SceneInfo.h"
#include "BlimpFactory.h"

class SceneDirector : public ISystem {
public:
	typedef std::shared_ptr<SceneDirector> ptr;

	SceneDirector(SceneInfo::ptr scene_info) : _scene_info(scene_info), difficulty(0.0f) { };
	virtual ~SceneDirector() { };

	virtual bool init(BumblebeeWorld::ptr world);
	virtual void destroy();
	virtual void tick();

	void init_scene();
	void fire_cannon();
	void blimp_collision(BumblebeeObject* obj, glm::vec3 contact_point);
	void collision_callback(unsigned int id_a, unsigned int id_b, glm::vec3 contact_point);
	bool save();

	unsigned int high_score;
	float difficulty;

private:
	float rotation;
	SceneInfo::ptr _scene_info;
	BumblebeeWorld::ptr _backup;

};

#endif