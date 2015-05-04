#ifndef _DemoFactory_H_
#define _DemoFactory_H_

#include <BumblebeeWorld.h>

class DemoFactory {
public:
	typedef std::shared_ptr<DemoFactory> ptr;

	DemoFactory(BumblebeeWorld::ptr world) : _world(world) { };
	~DemoFactory() { _world.reset(); };

	BumblebeeObject::ptr create_object(float x, float y, float z);
	BumblebeeObject::ptr create_cube(float x, float y, float z);
	BumblebeeObject::ptr create_cylinder(float x, float y, float z, float angle = 0.0f);
	BumblebeeObject::ptr create_plane(float x, float y, float z, float angle = 0.0f);
	BumblebeeLight::ptr create_light(LightType type, float x, float y, float z);
	
private:
	BumblebeeWorld::ptr _world;

};

#endif