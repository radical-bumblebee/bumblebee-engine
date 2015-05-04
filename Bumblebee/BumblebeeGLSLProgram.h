#ifndef _BumblebeeGLSLProgram_H_
#define _BumblebeeGLSLProgram_H_

#include "ShaderTypes.h"
#include "Logger.h"

#include "Dependencies/include/GL/glew.h"
#include "Dependencies/include/glm/mat4x4.hpp"
#include "Dependencies/include/glm/gtc/type_ptr.hpp"

#include <memory>
#include <stdio.h>
#include <iostream>

// Represents a pipeline of glsl shaders
class BumblebeeGLSLProgram {
public:
	typedef std::shared_ptr<BumblebeeGLSLProgram> ptr;

	BumblebeeGLSLProgram() :
		_program(0),
		_vertex(0),
		_geometry(0),
		_fragment(0),
		_vertex_src(nullptr),
		_geometry_src(nullptr),
		_fragment_src(nullptr)
		{ };
	~BumblebeeGLSLProgram() { 
		if (_vertex_src)
			delete _vertex_src;

		if (_geometry_src)
			delete _geometry_src;

		if (_fragment_src)
			delete _fragment_src;
	};
	
	bool set_shader_source(ShaderType shader_type, const char* shader_path);
	bool set_matrix(const char* name, const float* value, bool transpose);
	bool set_vec4(const char* name, const float* value);
	bool set_float(const char* name, const float value);
	bool set_int(const char* name, const int value);

	GLuint id();
	bool compile(bool transform = false);
	bool link();
	void enable();

private:
	GLuint _program;
	GLuint _vertex;
	GLuint _geometry;
	GLuint _fragment;

	char* _vertex_src;
	char* _geometry_src;
	char* _fragment_src;

};

#endif