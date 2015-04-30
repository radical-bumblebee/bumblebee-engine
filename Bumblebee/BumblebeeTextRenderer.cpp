#include "BumblebeeTextRenderer.h"

void BumblebeeTextRenderer::tick() {
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_program->enable();

	for (auto element : *_world->scene()->ui) {
		if (element->visible) {
			glEnable(GL_BLEND);
			glDisable(GL_CULL_FACE);

			BumblebeeFontAtlas::ptr atlas = _world->proxy->fonts[element->font];
						
			glBindTexture(GL_TEXTURE_2D, atlas->texture_id());
			_program->set_int("render_mode", 1);
			_program->set_vec4("object_color",	glm::value_ptr(element->color));

			glBindVertexArray(_vao_id);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_id);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

			const char* p;

			int window_w; int window_h;
			SDL_GetWindowSize(window, &window_w, &window_h);
			float sx = 2.0f / (float)window_w;
			float sy = 2.0f / (float)window_h;
			float width_percentage = element->x * window_w;
			float height_percentage = element->y * window_h;
			//float x = -1 + (element->x * window_w) * sx;
			//float y = 1 - (element->y * window_h) * sy;
			float x = -1 + element->x * sx;
			float y = 1 - (element->y + 20.0f) * sy;
			//float x = (element->x - 0.5f) * 2.0f;
			//float y = -(element->y - 0.5f) * 2.0f;

			float* string_data = new float[4 * (6 * strlen(element->text.c_str()))]();
			int c = 0;

			for (p = element->text.c_str(); *p; p++) {
				// Calculate the vertex and texture coordinates
				float x2 = x + atlas->font_cache[*p].bitmap_left * sx;
				float y2 = -y - atlas->font_cache[*p].bitmap_top * sy;
				float w = atlas->font_cache[*p].bitmap_width * sx;
				float h = atlas->font_cache[*p].bitmap_height * sy;

				// Advance cursor to the next character
				x += atlas->font_cache[*p].advance_x * sx;
				y += atlas->font_cache[*p].advance_y * sy;

				// Skip empty glyphs
				if (!w || !h) {
					continue;
				}

				// Populate vertex data
				string_data[c++] = x2;
				string_data[c++] = -y2;
				string_data[c++] = atlas->font_cache[*p].texture_offset_x;
				string_data[c++] = atlas->font_cache[*p].texture_offset_y;

				string_data[c++] = x2 + w;
				string_data[c++] = -y2;
				string_data[c++] = atlas->font_cache[*p].texture_offset_x + atlas->font_cache[*p].bitmap_width / atlas->width;
				string_data[c++] = atlas->font_cache[*p].texture_offset_y;

				string_data[c++] = x2;
				string_data[c++] = -y2 - h;
				string_data[c++] = atlas->font_cache[*p].texture_offset_x;
				string_data[c++] = atlas->font_cache[*p].texture_offset_y + atlas->font_cache[*p].bitmap_height / atlas->height;

				string_data[c++] = x2 + w;
				string_data[c++] = -y2;
				string_data[c++] = atlas->font_cache[*p].texture_offset_x + atlas->font_cache[*p].bitmap_width / atlas->width;
				string_data[c++] = atlas->font_cache[*p].texture_offset_y;

				string_data[c++] = x2;
				string_data[c++] = -y2 - h;
				string_data[c++] = atlas->font_cache[*p].texture_offset_x;
				string_data[c++] = atlas->font_cache[*p].texture_offset_y + atlas->font_cache[*p].bitmap_height / atlas->height;

				string_data[c++] = x2 + w;
				string_data[c++] = -y2 - h;
				string_data[c++] = atlas->font_cache[*p].texture_offset_x + atlas->font_cache[*p].bitmap_width / atlas->width;
				string_data[c++] = atlas->font_cache[*p].texture_offset_y + atlas->font_cache[*p].bitmap_height / atlas->height;
			}

			glBufferData(GL_ARRAY_BUFFER, c * sizeof(float), string_data, GL_STATIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, c);

			glDisableVertexAttribArray(0);
			delete[] string_data;
		}
	}
}

void BumblebeeTextRenderer::set_program(BumblebeeGLSLProgram::ptr program) {
	_program = program;
}

FT_Face BumblebeeTextRenderer::font_added(const char* font_name) {
	FT_Face face;
	if (FT_New_Face(_freetype, font_name, 0, &face)) {
		Logger::get()->log("Could not create new freetype face");
	}
	return face;
}


bool BumblebeeTextRenderer::init(BumblebeeWorld::ptr world) {
	_world = world;

	if (FT_Init_FreeType(&_freetype)) {
		Logger::get()->log("Could not initialize freetype library");
	}

	glGenVertexArrays(1, &_vao_id);
	glGenBuffers(1, &_vbo_id);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_world->proxy->font_added_callback = std::bind(&BumblebeeTextRenderer::font_added, this, std::placeholders::_1);

	return true;
}

void BumblebeeTextRenderer::destroy() {
	FT_Done_FreeType(_freetype);

	_program.reset();
	_world.reset();
}