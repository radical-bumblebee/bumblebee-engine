#ifndef _SceneDirector_H_
#define _SceneDirector_H_

#include <ISystem.h>
#include "SceneInfo.h"
#include "DemoFactory.h"

#include <random>
#include <queue>

#define PLANE_LENGTH 7.5f
#define SEGMENT_LENGTH PLANE_LENGTH * 2.0f
#define PILLAR_LENGTH SEGMENT_LENGTH / 3.0f

class SceneDirector : public ISystem {
public:
	typedef std::shared_ptr<SceneDirector> ptr;

	SceneDirector(SceneInfo::ptr scene_info, DemoFactory::ptr factory) : _scene_info(scene_info), _factory(factory), _segment_end(0.0f), _segment_begin(0.0f), _segment_index(0), score(0) { };
	virtual ~SceneDirector() { };

	virtual bool init(BumblebeeWorld::ptr world);
	virtual void destroy();
	virtual void tick();

	void generate_segment(unsigned int num_segments = 1, bool generate_pillars = true);

	unsigned int score;

private:
	BumblebeeWorld::ptr _world;
	SceneInfo::ptr _scene_info;
	DemoFactory::ptr _factory;

	std::queue<BumblebeeObject::ptr> _pillars;
	float _segment_end;
	float _segment_begin;
	unsigned int _segment_index;

};

#endif