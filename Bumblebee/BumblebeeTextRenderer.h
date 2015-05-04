#ifndef _BumblebeeTextRenderer_H_
#define _BumblebeeTextRenderer_H_

#include "ISystem.h"
#include "Logger.h"
#include "BumblebeeGLSLProgram.h"

#include "Dependencies/include/GL/glew.h"
#include "Dependencies/include/SDL/SDL.h"
#include "Dependencies/include/SDL/SDL_opengl.h"
#include "Dependencies/include/glm/gtc/type_ptr.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

// Manages text rendering using modern opengl
class BumblebeeTextRenderer : public ISystem {
public:
	typedef std::shared_ptr<BumblebeeTextRenderer> ptr;

	BumblebeeTextRenderer() { };
	virtual ~BumblebeeTextRenderer() { };

	virtual bool init(BumblebeeWorld::ptr world);
	virtual void destroy();
	virtual void tick();

	void set_program(BumblebeeGLSLProgram::ptr program);
	FT_Face font_added(const char* font_name);

	SDL_Window* window;

private:
	BumblebeeGLSLProgram::ptr _program;

	FT_Library _freetype;
	FT_Face _times;	

	unsigned int _vbo_id;
	unsigned int _vao_id;

};

#endif