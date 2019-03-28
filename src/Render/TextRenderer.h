#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <map>

#include "Headers.h"
#include "Shader.h"

struct Character {
	GLuint TextureID; // ID handle of the glyph texture
	glm::ivec2 Size; // Size of glyph
	glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance; // Horizontal offset to advance to next glyph
};

class TextRenderer {
public:
	std::map<GLchar, Character> characters;
	Shader* textShader;
	TextRenderer(GLfloat defaultScale = 1.0f);
	void load(std::string font, GLuint fontSize);
	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale = 1.0f, bool center = true,
	                glm::vec3 color = glm::vec3(1.0f));
private:
	GLfloat defaultScale;
	GLuint vao, vbo;
};

#endif
