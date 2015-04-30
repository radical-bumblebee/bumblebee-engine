#ifndef _ISystem_H_
#define _ISystem_H_

#include "SystemTypes.h"
#include "BumblebeeWorld.h"

#include <memory>

class ISystem {
public:
	typedef std::shared_ptr<ISystem> ptr;

	ISystem() { };
	virtual ~ISystem() { };

	virtual bool init(BumblebeeWorld::ptr world) = 0;
	virtual void destroy() = 0;
	virtual void tick() = 0;

	int priority;
	SystemType type;

protected:
	BumblebeeWorld::ptr _world;

};

#endif