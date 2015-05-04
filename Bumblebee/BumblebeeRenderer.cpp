#include "BumblebeeRenderer.h"

void APIENTRY gl_debug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

// Renders all visible objects
void BumblebeeRenderer::tick() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	_program->enable();

	// Update view matrix from camera
	if (_world->scene()->camera()) {
		_world->scene()->camera()->view_matrix = glm::lookAt(_world->scene()->camera()->eye,
			_world->scene()->camera()->target,
			_world->scene()->camera()->up);
	}

	_program->set_float("game_over", _world->config->game_over);

	// Update lighting uniforms
	if (!_world->scene()->lights->empty()) {
		_program->set_int("num_lights", static_cast<int>(_world->scene()->lights->size()));
		std::string light_prefix;
		for (size_t i = 0; i < _world->scene()->lights->size(); ++i) {
			light_prefix = "lights[";
			light_prefix.append(std::to_string(i));
			_program->set_vec4((light_prefix + "].position").c_str(), glm::value_ptr(_world->scene()->lights->at(i)->position));
			_program->set_vec4((light_prefix + "].intensities").c_str(), glm::value_ptr(_world->scene()->lights->at(i)->intensities));
			_program->set_vec4((light_prefix + "].cone_direction").c_str(), glm::value_ptr(_world->scene()->lights->at(i)->cone_direction));
			_program->set_float((light_prefix + "].attenuation").c_str(), _world->scene()->lights->at(i)->attenuation);
			_program->set_float((light_prefix + "].ambient").c_str(), _world->scene()->lights->at(i)->ambient);
			_program->set_float((light_prefix + "].cone_angle").c_str(), _world->scene()->lights->at(i)->cone_angle);
		}
	}
	
	for (auto obj = _world->scene()->objects->begin(); obj != _world->scene()->objects->end(); ++obj) {
		if (obj->model->info->visible) {
			glm::mat4 world_matrix(1.0f);

			world_matrix = glm::translate(world_matrix, glm::vec3(obj->spatial->position.x, obj->spatial->position.y, obj->spatial->position.z));
			world_matrix *= glm::mat4_cast(obj->spatial->orientation);
			world_matrix = glm::scale(world_matrix, obj->spatial->scale);

			// Update transformation uniforms
			_program->set_matrix("view_matrix", glm::value_ptr(_world->scene()->camera()->view_matrix), false);
			_program->set_matrix("world_matrix", glm::value_ptr(world_matrix), false);

			// Update object uniforms
			_program->set_vec4("camera_position", glm::value_ptr(_world->scene()->camera()->eye));
			_program->set_vec4("object_color", glm::value_ptr(obj->model->info->color));
			_program->set_float("object_shininess", obj->model->info->shininess);
			_program->set_float("object_transparency", obj->model->info->transparency);
			
			// Set rendering mode if object is a skybox
			if (obj->model->texture == _world->proxy->textures["skybox"]) {
				_program->set_int("render_mode", 2);
				glCullFace(GL_FRONT);
				glDepthFunc(GL_LEQUAL);
				glBindTexture(GL_TEXTURE_CUBE_MAP, obj->model->texture);
			}
			else {
				_program->set_int("render_mode", 0);
				glBindTexture(GL_TEXTURE_2D, obj->model->texture);
			}

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glBindVertexArray(obj->model->info->vbo_id);
			glBindBuffer(GL_ARRAY_BUFFER, obj->model->info->vao_id);			
			
			// Updates vertices if object is a dynamic body
			if (obj->model->info->update_mesh) {
				float* mesh = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, obj->model->info->num_vertices, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
				for (int i = 0; i < obj->model->info->num_vertices; ++i) {
					mesh[i * 4] = obj->model->info->vertices.at(i).x;
					mesh[(i * 4) + 1] = obj->model->info->vertices.at(i).y;
					mesh[(i * 4) + 2] = obj->model->info->vertices.at(i).z;
				}
				glUnmapBuffer(GL_ARRAY_BUFFER);
			}

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			// Draw object
			if (obj->model->info->num_faces > 0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->model->info->ib_id);
				glDrawElements(GL_TRIANGLES, obj->model->info->num_faces * 3, GL_UNSIGNED_INT, (GLvoid*)0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
			else {
				// No index buffer
				glDrawArrays(GL_POINTS, 0, obj->model->info->num_vertices);
			}

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);

			if (obj->model->texture == _world->proxy->textures["skybox"]) {
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				glCullFace(GL_BACK);
				glDepthFunc(GL_LESS);
			}
		}
	}	

	// Tick subsystems
	_particle_renderer->tick();
	_text_renderer->tick();

	// Present
	SDL_GL_SwapWindow(window);
}

// Set application icon
void BumblebeeRenderer::set_app_icon() {
	SDL_Surface* surface = SDL_LoadBMP("assets/icons/radbee.bmp");
	SDL_SetSurfaceAlphaMod(surface, 24);
	surface->format->Amask = 0xFF00000;
	surface->format->Ashift = 24;
	if (surface) {
		SDL_SetWindowIcon(window, surface);
		SDL_FreeSurface(surface);
	}
}

// Initializes renderer pipeline
bool BumblebeeRenderer::init_shaders() {
	_program = std::make_shared<BumblebeeGLSLProgram>();

	if (!_program->set_shader_source(VERTEX_SHADER, "assets/shaders/Bumblebee.vertex.glsl")) {
		std::cout << "Error setting vertex shader source" << std::endl;
		return false;
	}
	if (!_program->set_shader_source(FRAGMENT_SHADER, "assets/shaders/Bumblebee.fragment.glsl")) {
		std::cout << "Error setting fragment shader source" << std::endl;
		return false;
	}
	if (!_program->compile()) {
		std::cout << "Error compiling shaders" << std::endl;
		return false;
	}

	_program->enable();
	_program->set_matrix("projection_matrix", glm::value_ptr(_world->projection_matrix), false);

	return true;
}

// Load a texture image with gli
unsigned int BumblebeeRenderer::load_texture(GLenum texture_target, const char* src) {
	gli::texture2D texture(gli::load_dds(src));
	if (texture_target == GL_TEXTURE_CUBE_MAP_POSITIVE_X) {
		glTexStorage2D(GL_TEXTURE_CUBE_MAP, static_cast<GLint>(texture.levels()),
			static_cast<GLenum>(gli::internal_format(texture.format())),
			static_cast<GLsizei>(texture.dimensions().x), static_cast<GLsizei>(texture.dimensions().y));
	}

	if (texture_target != GL_TEXTURE_CUBE_MAP_POSITIVE_X && texture_target != GL_TEXTURE_CUBE_MAP_NEGATIVE_X && texture_target != GL_TEXTURE_CUBE_MAP_POSITIVE_Y && texture_target != GL_TEXTURE_CUBE_MAP_NEGATIVE_Y && texture_target != GL_TEXTURE_CUBE_MAP_POSITIVE_Z && texture_target != GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) {
		glTexStorage2D(texture_target, static_cast<GLint>(texture.levels()),
			static_cast<GLenum>(gli::internal_format(texture.format())),
			static_cast<GLsizei>(texture.dimensions().x), static_cast<GLsizei>(texture.dimensions().y));
	}

	if (gli::is_compressed(texture.format())) {
		for (gli::texture2D::size_type level = 0; level < texture.levels(); ++level) {
			glCompressedTexSubImage2D(texture_target,
				static_cast<GLint>(level), 0, 0,
				static_cast<GLsizei>(texture[level].dimensions().x),
				static_cast<GLsizei>(texture[level].dimensions().y),
				static_cast<GLenum>(gli::external_format(texture.format())),
				static_cast<GLsizei>(texture[level].size()),
				texture[level].data());
		}
	}
	else {
		for (gli::texture2D::size_type level = 0; level < texture.levels(); ++level) {
			glTexSubImage2D(texture_target,
				static_cast<GLint>(level), 0, 0,
				static_cast<GLsizei>(texture[level].dimensions().x),
				static_cast<GLsizei>(texture[level].dimensions().y),
				static_cast<GLenum>(gli::external_format(texture.format())),
				static_cast<GLenum>(gli::type_format(texture.format())),
				texture[level].data());
		}
	}

	return texture.levels();
}

// Callback for when a skybox is loaded, copies it into the gl buffers
unsigned int BumblebeeRenderer::skybox_added(const char* pos_x, const char* neg_x, const char* pos_y, const char* neg_y, const char* pos_z, const char* neg_z) {
	glActiveTexture(GL_TEXTURE0);
	GLuint texture_name = 0;
	glGenTextures(1, &texture_name);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_name);

	// Load all sides
	load_texture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, pos_x);
	load_texture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, neg_x);
	load_texture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, pos_y);
	load_texture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, neg_y);
	load_texture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, pos_z);
	load_texture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, neg_z);

	// Filtering options
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return texture_name;
}

// Callback for when a texture is loaded, copies it into gl buffers
unsigned int BumblebeeRenderer::texture_added(const char* src) {
	glActiveTexture(GL_TEXTURE0);
	GLuint texture_name = 0;
	GLenum texture_target = GL_TEXTURE_2D;
	glGenTextures(1, &texture_name);
	glBindTexture(texture_target, texture_name);

	unsigned int levels = load_texture(texture_target, src);

	glTexParameteri(texture_target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(texture_target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(levels - 1));
	glTexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture_name;
}

// Callback for when a model is loaded, copies it into the gl buffers
void BumblebeeRenderer::model_added(ModelInfo::ptr model) {
	if (!model) {
		Logger::get()->log("Model not found");
	}

	// Copy data in gl friendly format
	float* array_positions = new float[model->num_vertices * 4]();
	float* array_normals = new float[model->num_vertices * 4]();
	float* array_uv = new float[model->num_vertices * 4]();

	int i = 0;
	for (auto vtx : model->vertices) {
		int idx_1 = (i * 4);
		int idx_2 = (i * 4) + 1;
		int idx_3 = (i * 4) + 2;
		int idx_4 = (i * 4) + 3;

		array_positions[idx_1] = vtx.x;
		array_positions[idx_2] = vtx.y;
		array_positions[idx_3] = vtx.z;
		array_positions[idx_4] = 1.0f;

		array_normals[idx_1] = vtx.nx;
		array_normals[idx_2] = vtx.ny;
		array_normals[idx_3] = vtx.nz;
		array_normals[idx_4] = 1.0f;

		array_uv[idx_1] = vtx.uv_x;
		array_uv[idx_2] = vtx.uv_y;
		array_uv[idx_3] = 0.0f;
		array_uv[idx_4] = 0.0f;

		++i;
	}

	// Generate buffers
	glGenVertexArrays(1, &model->vbo_id);
	glBindVertexArray(model->vbo_id);

	glGenBuffers(1, &model->vao_id);
	glBindBuffer(GL_ARRAY_BUFFER, model->vao_id);

	int type_size = (sizeof(float) * 4) * model->num_vertices;
	int buffer_size = 3 * type_size;

	// Copy data into buffers
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STREAM_DRAW);

	unsigned int offset_position = 0;
	unsigned int offset_normal = offset_position + type_size;
	unsigned int offset_uv = offset_normal + type_size;

	glBufferSubData(GL_ARRAY_BUFFER, offset_position, type_size, array_positions);
	glBufferSubData(GL_ARRAY_BUFFER, offset_normal, type_size, array_normals);
	glBufferSubData(GL_ARRAY_BUFFER, offset_uv, type_size, array_uv);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset_normal);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset_uv);

	// Copy in index data if applicable
	if (model->num_faces > 0) {
		auto range = model->faces.size() * 3;
		unsigned int* array_indices = new unsigned int[range];

		for (size_t i = 0; i < model->faces.size(); ++i) {
			int idx_1 = (i * 3);
			int idx_2 = (i * 3) + 1;
			int idx_3 = (i * 3) + 2;

			array_indices[idx_1] = model->faces[i].a;
			array_indices[idx_2] = model->faces[i].b;
			array_indices[idx_3] = model->faces[i].c;
		}

		glGenBuffers(1, &model->ib_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ib_id);

		int index_size = (model->num_faces * 3) * sizeof(GLuint);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, array_indices, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		delete[] array_indices;
	}

	// Cleanup
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	delete[] array_positions;
	delete[] array_normals;
	delete[] array_uv;
}

// Resize viewport
void BumblebeeRenderer::resize(int w, int h) {
	glViewport(0, 0, w, h);
	_world->window_w = (float)w;
	_world->window_h = (float)h;
	_world->projection_matrix = glm::perspective(FOV, (float)w / h, NEAR_PLANE, FAR_PLANE);
}

// Initializes system
bool BumblebeeRenderer::init(BumblebeeWorld::ptr world) {
	_world = world;

	this->priority = 0;
	this->type = BUMBLEBEE_RENDERER;

	// Initializes SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		std::string error = SDL_GetError();
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef _DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	_world->window_w = WINDOW_WIDTH;
	_world->window_h = WINDOW_HEIGHT;

	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (GLuint)_world->window_w, (GLuint)_world->window_h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		std::string error = SDL_GetError();
		return false;
	}

	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_ShowCursor(0); // Disables cursor
	//set_app_icon();

	_context = SDL_GL_CreateContext(window);
	if (_context == NULL) {
		std::string error = SDL_GetError();
		return false;
	}

	// Initializes glew
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::string error = (const char *)glewGetErrorString(glewError);
		return false;
	}
	glGetError();

#ifdef _DEBUG
	// Enable gl debug callback
	if (GLEW_ARB_debug_output) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(gl_debug, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			&unusedIds,
			true);
	}
#endif

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);

	glPointSize(5.0f);

	glViewport(0, 0, (GLuint)_world->window_w, (GLuint)_world->window_h);
	_world->projection_matrix = glm::perspective(FOV, _world->window_w / _world->window_h, NEAR_PLANE, FAR_PLANE);

	this->init_shaders();

	// Sets buffer callbacks
	_world->proxy->model_added_callback = std::bind(&BumblebeeRenderer::model_added, this, std::placeholders::_1);
	_world->proxy->texture_added_callback = std::bind(&BumblebeeRenderer::texture_added, this, std::placeholders::_1);
	_world->proxy->skybox_added_callback = std::bind(&BumblebeeRenderer::skybox_added, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

	// Initialize particle renderer
	_particle_renderer = std::make_shared<BumblebeeParticleRenderer>();
	_particle_renderer->init(world);
	
	// Initialize text renderer
	_text_renderer = std::make_shared<BumblebeeTextRenderer>();
	_text_renderer->init(world);
	_text_renderer->set_program(_program);
	_text_renderer->window = window;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	SDL_GL_SwapWindow(window);

	return true;
}

// Destroy system
void BumblebeeRenderer::destroy() {
	_particle_renderer->destroy();
	_text_renderer->destroy();

	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(window);
	window = nullptr;
	SDL_Quit();

	_program.reset();
	_particle_renderer.reset();
	_text_renderer.reset();
	_world.reset();
}

// OpenGL debug callback
void APIENTRY gl_debug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) { return; }

	std::cout << "---------------------opengl-callback-start------------" << std::endl;
	std::cout << "message: " << message << std::endl;
	std::cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cout << "OTHER";
		break;
	}
	std::cout << std::endl;

	std::cout << "severity: ";
	switch (severity){
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cout << "INFO";
		break;
	}
	std::cout << std::endl;
	std::cout << "---------------------opengl-callback-end--------------" << std::endl << std::endl;
}