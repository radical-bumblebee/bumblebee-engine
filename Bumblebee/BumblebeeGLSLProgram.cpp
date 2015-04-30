#include "BumblebeeGLSLProgram.h"

// Compiles a pair of shaders
bool BumblebeeGLSLProgram::compile(bool transform) {
	_program = glCreateProgram();

	if (_vertex_src) {
		_vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(_vertex, 1, &_vertex_src, 0);
		glCompileShader(_vertex);
		glAttachShader(_program, _vertex);
	}
	else {
		Logger::get()->log("Failed to attach vertex shader");
	}

	if (_geometry_src) {
		_geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(_geometry, 1, &_geometry_src, 0);
		glCompileShader(_geometry);
		glAttachShader(_program, _geometry);
	}

	if (_fragment_src) {
		_fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(_fragment, 1, &_fragment_src, 0);
		glCompileShader(_fragment);
		glAttachShader(_program, _fragment);
	}
	else {
		Logger::get()->log("Failed to attach fragment shader");
	}

	if (transform) {
		const GLchar* geometry_varyings[4];
		geometry_varyings[0] = "fs_position";
		geometry_varyings[1] = "fs_velocity";
		geometry_varyings[2] = "fs_type";
		geometry_varyings[3] = "fs_age";

		glTransformFeedbackVaryings(_program, 4, geometry_varyings, GL_INTERLEAVED_ATTRIBS);
	}

	link();

	return true;
}

bool BumblebeeGLSLProgram::link() {
	glLinkProgram(_program);

	GLint link_success = 0;
	glGetProgramiv(_program, GL_LINK_STATUS, &link_success);

	// Linking failed, report something useful, hopefully
	if (!link_success) {
		GLint buffer_length = 0;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &buffer_length);
		if (buffer_length > 0) {
			char* buffer = new char[buffer_length];
			if (buffer) {
				glGetProgramInfoLog(_program, buffer_length, NULL, buffer);
				std::cout << "Error linking program\n" << buffer << std::endl;
				delete[] buffer;
			}
		}

		return false;
	}

	return true;
}

// Sets a uniform matrix in the shader
bool BumblebeeGLSLProgram::set_matrix(const char* name, const float* value, bool transpose) {
	GLint location = glGetUniformLocation(_program, name);

	if (location < 0) {
		return false;
	}

	glUniformMatrix4fv(location, 1, transpose, value);
	
	return true;
}

// Sets a uniform vec4 in the shader
bool BumblebeeGLSLProgram::set_vec4(const char* name, const float* value) {
	GLint location = glGetUniformLocation(_program, name);

	if (location < 0) {
		return false;
	}

	glUniform4f(location,
		value[0],
		value[1],
		value[2],
		value[3]);

	return true;
}

// Sets a uniform float in the shader
bool BumblebeeGLSLProgram::set_float(const char* name, const float value) {
	GLint location = glGetUniformLocation(_program, name);

	if (location < 0) {
		return false;
	}

	glUniform1f(location, value);

	return true;
}

// Sets a uniform int in the shader
bool BumblebeeGLSLProgram::set_int(const char* name, const int value) {
	GLint location = glGetUniformLocation(_program, name);

	if (location < 0) {
		return false;
	}

	glUniform1i(location, value);

	return true;
}

// Reads shader source files
bool BumblebeeGLSLProgram::set_shader_source(ShaderType shader_type, const char* shader_path) {
	FILE* file = nullptr;
	if (fopen_s(&file, shader_path, "rb") != 0) {
		return false;
	}

	int length = 0;
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	switch (shader_type) {
	case VERTEX_SHADER:
		_vertex_src = new char[length + 1];
		fread(_vertex_src, 1, length, file);
		_vertex_src[length] = '\0';
		break;
	case GEOMETRY_SHADER:
		_geometry_src = new char[length + 1];
		fread(_geometry_src, 1, length, file);
		_geometry_src[length] = '\0';
		break;
	case FRAGMENT_SHADER:
		_fragment_src = new char[length + 1];
		fread(_fragment_src, 1, length, file);
		_fragment_src[length] = '\0';
		break;
	}

	fclose(file);

	return true;
}

// Sets shader to active
void BumblebeeGLSLProgram::enable() {
	glUseProgram(_program);
}

// Returns shader id
GLuint BumblebeeGLSLProgram::id() {
	return _program;
}