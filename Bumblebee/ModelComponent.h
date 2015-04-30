#ifndef _ModelComponent_H_
#define _ModelComponent_H_

#include "IComponent.h"
#include "ModelInfo.h"

// Contains model data
class ModelComponent : public IComponent {
public:
	typedef std::shared_ptr<ModelComponent> ptr;
	typedef std::unique_ptr<ModelComponent> uptr;

	ModelComponent() : texture(0) { 
		this->type = MODEL_COMPONENT;
	};
	ModelComponent(ModelInfo::ptr info, unsigned int texture = 0) : info(info), texture(texture) {
		this->type = MODEL_COMPONENT;
	};
	virtual ~ModelComponent() {
		info.reset();
	};

	ModelInfo::ptr info;
	unsigned int texture;
};

#endif