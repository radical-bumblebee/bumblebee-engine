#ifndef _SceneInfo_H_
#define _SceneInfo_H_

#include <memory>

class SceneInfo {
public:
	typedef std::shared_ptr<SceneInfo> ptr;

	SceneInfo() : scene_id(0) { };
	~SceneInfo() { };

	unsigned int scene_id;
};

#endif