#ifndef _IComponent_H_
#define _IComponent_H_

#include <memory>

#include "ComponentTypes.h"

class BumblebeeObject;

// Component interface, we might try to find a cleaner way to do this 
class IComponent {
public:
	typedef std::shared_ptr<IComponent> ptr;

	IComponent() { };
	virtual ~IComponent() { };
	
	ComponentType type;

};

#endif