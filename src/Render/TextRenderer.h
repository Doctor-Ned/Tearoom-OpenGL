#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <map>

#include "Global.h"
#include "Render/Shader.h"
#include "Ui/UiElement.h"

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
	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scaleX, GLfloat scaleY, bool center, glm::vec3 color);
	void renderText(std::string text, GLfloat x, GLfloat y, glm::vec2 scale, bool center, glm::vec3 color);
	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale = 1.0f, bool center = true, glm::vec3 color = glm::vec3(1.0f));
	void renderText(std::string text, glm::vec2 position, GLfloat scaleX, GLfloat scaleY, bool center, glm::vec3 color);
	void renderText(std::string text, glm::vec2 position, glm::vec2 scale, bool center, glm::vec3 color);
	void renderText(std::string text, glm::vec2 position, GLfloat scale = 1.0f, bool center = true, glm::vec3 color = glm::vec3(1.0f));
	void updateProjection();
	GLuint getFontSize();
	glm::vec2 getScaleToFitSize(std::string text, glm::vec2 size, UiRescaleMode rescaleMode);
	glm::vec2 getTextSize(std::string text, GLfloat scale = 1.0f);
	glm::vec2 getTextSize(std::string text, GLfloat scaleX, GLfloat scaleY);
	glm::vec2 getTextSize(std::string text, glm::vec2 scale);
private:
	GLfloat defaultScale;
	GLuint vao, vbo;
	GLuint fontSize = 0;
};

#endif
