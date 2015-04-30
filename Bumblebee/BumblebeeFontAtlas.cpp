#include "BumblebeeFontAtlas.h"

void BumblebeeFontAtlas::update() {
	FT_Set_Pixel_Sizes(_face, 0, _font_size);
	FT_GlyphSlot g = _face->glyph;

	memset(font_cache, 0, sizeof(font_cache));

	unsigned int roww = 0;
	unsigned int rowh = 0;
	width = 0;
	height = 0;

	// Find minimum size for a texture holding all visible ASCII characters 
	for (int i = 32; i < 128; i++) {
		if (FT_Load_Char(_face, i, FT_LOAD_RENDER)) {
			fprintf(stderr, "Loading character %c failed!\n", i);
			continue;
		}
		if (roww + g->bitmap.width + 1 >= 1024) {
			width = glm::max(width, roww);
			height += rowh;
			roww = 0;
			rowh = 0;
		}
		roww += g->bitmap.width + 1;
		rowh = glm::max(rowh, g->bitmap.rows);
	}

	width = glm::max(width, roww);
	height += rowh;

	glGenTextures(1, &_texture_id);
	glBindTexture(GL_TEXTURE_2D, _texture_id);

	// Empty texture for sub data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	// Ensure 1 byte alignment for font packing
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Clamp texture to edges
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Linear filtering 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int ox = 0;
	int oy = 0;
	rowh = 0;

	for (int i = 32; i < 128; i++) {
		if (FT_Load_Char(_face, i, FT_LOAD_RENDER)) {
			continue;
		}

		if (ox + g->bitmap.width + 1 >= 1024) {
			oy += rowh;
			rowh = 0;
			ox = 0;
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
		font_cache[i].advance_x = (float)(g->advance.x >> 6);
		font_cache[i].advance_y = (float)(g->advance.y >> 6);

		font_cache[i].bitmap_width = (float)g->bitmap.width;
		font_cache[i].bitmap_height = (float)g->bitmap.rows;

		font_cache[i].bitmap_left = (float)g->bitmap_left;
		font_cache[i].bitmap_top = (float)g->bitmap_top;

		font_cache[i].texture_offset_x = ox / (float)width;
		font_cache[i].texture_offset_y = oy / (float)height;

		rowh = glm::max(rowh, g->bitmap.rows);
		ox += g->bitmap.width + 1;
	}
}

void BumblebeeFontAtlas::set_font_size(unsigned int font_size) {
	_font_size = font_size;
	
	update();
}

BumblebeeFontAtlas::BumblebeeFontAtlas(FT_Face face) {
	_face = face;
	_font_size = DEFAULT_FONT_SIZE;
	
	update();
}

BumblebeeFontAtlas::~BumblebeeFontAtlas() {
	glDeleteTextures(1, &_texture_id);
}