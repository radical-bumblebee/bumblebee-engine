#ifndef _BumblebeeUIElement_H_
#define _BumblebeeUIElement_H_

#include "Dependencies/include/glm/vec4.hpp"

#include <memory>
#include <string>

// Represents a 2D text object
class BumblebeeUIElement {
public:
	typedef std::shared_ptr<BumblebeeUIElement> ptr;

	BumblebeeUIElement(std::string text, std::string font, float x, float y, glm::vec4* color) : text(text), font(font), x(x), y(y), visible(true), color(*color) { };
	~BumblebeeUIElement() { };

	std::string text;
	std::string font;
	float x;
	float y;
	glm::vec4 color;

	bool visible;

};

#endif