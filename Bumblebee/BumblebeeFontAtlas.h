#ifndef _BumblebeeFontAtlas_H_
#define _BumblebeeFontAtlas_H_

#include "Dependencies/include/GL/glew.h"
#include "Dependencies/include/glm/vec4.hpp"
#include "Dependencies/include/glm/glm.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

#include <memory>

#define DEFAULT_FONT_SIZE 20

class BumblebeeFontAtlas {
public:
	typedef std::shared_ptr<BumblebeeFontAtlas> ptr;

	BumblebeeFontAtlas(FT_Face face);
	~BumblebeeFontAtlas();

	class CachedKey {
	public:
		CachedKey() : advance_x(0.0f), advance_y(0.0f), bitmap_width(0.0f), bitmap_height(0.0f),
			bitmap_left(0.0f), bitmap_top(0.0f), texture_offset_x(0.0f), texture_offset_y(0.0f) { };
		~CachedKey() { };

		float advance_x;
		float advance_y;

		float bitmap_width;
		float bitmap_height;

		float bitmap_left;
		float bitmap_top;

		float texture_offset_x;
		float texture_offset_y;
	};

	GLuint texture_id() { return _texture_id; };
	void set_font_size(unsigned int font_size);

	CachedKey font_cache[128];
	unsigned int width;
	unsigned int height;

private:
	unsigned int _font_size;
	GLuint _texture_id;
	FT_Face _face;

	void update();

};

#endif