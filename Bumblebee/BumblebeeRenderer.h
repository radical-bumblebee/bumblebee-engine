#ifndef _BumblebeeRenderer_H_
#define _BumblebeeRenderer_H_

#include "ISystem.h"
#include "BumblebeeGLSLProgram.h"
#include "SpatialComponent.h"
#include "ModelComponent.h"
#include "BulletComponent.h"
#include "BumblebeeTextRenderer.h"
#include "BumblebeeParticleRenderer.h"
#include "Logger.h"

#include "Dependencies/include/SDL/SDL.h"
#include "Dependencies/include/SDL/SDL_opengl.h"
#include "Dependencies/include/SDL/SDL_image.h"
#include "Dependencies/include/glm/vec3.hpp"
#include "Dependencies/include/glm/gtc/matrix_transform.hpp"
#include "Dependencies/include/glm/gtc/type_ptr.hpp"
#include "Dependencies/include/gli/gli.hpp"

#include <string>
#include <vector>
#include <memory>

#define WINDOW_TITLE "Grand Blimp Bastards"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define NEAR_PLANE 0.5f
#define FAR_PLANE 200.0f
#define FOV 45.0f

class BumblebeeRenderer : public ISystem {
public:
	typedef std::shared_ptr<BumblebeeRenderer> ptr;

	BumblebeeRenderer() { };
	virtual ~BumblebeeRenderer() { };

	virtual bool init(BumblebeeWorld::ptr world);
	virtual void destroy();
	virtual void tick();

	// callbacks
	void model_added(ModelInfo::ptr model);
	unsigned int texture_added(const char* src);
	unsigned int skybox_added(const char* pos_x, const char* neg_x, const char* pos_y, const char* neg_y, const char* pos_z, const char* neg_z);

	void resize(int w, int h);
	void set_app_icon();
	bool init_shaders();
	unsigned int load_texture(GLenum texture_target, const char* src);

	SDL_Window* window;

private:
	BumblebeeGLSLProgram::ptr _program;
	BumblebeeTextRenderer::ptr _text_renderer;
	BumblebeeParticleRenderer::ptr _particle_renderer;

	SDL_GLContext _context;

};

#endif