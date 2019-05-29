#include <iostream>
#include "Render/TextRenderer.h"
#include "freetype/ftparams.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"


TextRenderer::TextRenderer(GLfloat defaultScale) {
	// Load and configure shader
	this->defaultScale = defaultScale;
	Shader *textShader = AssetManager::getInstance()->getShader(STUiText);
	textShader->use();
	textShader->setInt("text", 0);
	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TextRenderer::load(std::string font, GLuint fontSize) {
	this->fontSize = fontSize;
	// First clear the previously loaded Characters
	characters.clear();
	// Then initialize and load the FreeType library
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) // All functions return a value different than 0 whenever an error occurred
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, font.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, fontSize);
	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Then for the first 128 ASCII characters, pre-load/compile their characters and store them
	for (GLubyte c = 0; c < 128; c++) // lol see what I did there 
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void TextRenderer::renderText(Shader* shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, UiAnchor anchor, glm::vec3 color) {
	renderText(shader, text, x, y, scale, scale, anchor, color);
}

void TextRenderer::renderText(Shader* shader, std::string text, glm::vec2 position, GLfloat scaleX, GLfloat scaleY, UiAnchor anchor, glm::vec3 color) {
	renderText(shader, text, position.x, position.y, scaleX, scaleY, anchor, color);
}

void TextRenderer::renderText(Shader* shader, std::string text, glm::vec2 position, glm::vec2 scale, UiAnchor anchor, glm::vec3 color) {
	renderText(shader, text, position.x, position.y, scale, anchor, color);
}

void TextRenderer::renderText(Shader* shader, std::string text, glm::vec2 position, GLfloat scale, UiAnchor anchor, glm::vec3 color) {
	renderText(shader, text, position.x, position.y, scale, anchor, color);
}

void TextRenderer::renderText(Shader* shader, std::string text, GLfloat x, GLfloat y, glm::vec2 scale, UiAnchor anchor, glm::vec3 color) {
	renderText(shader, text, x, y, scale.x, scale.y, anchor, color);
}

void TextRenderer::renderText(Shader* shader, std::string text, GLfloat x, GLfloat y, GLfloat scaleX, GLfloat scaleY, UiAnchor anchor, glm::vec3 color) {
	shader->setColor(glm::vec4(color, 1.0f));
	renderText(shader, text, x, y, scaleX, scaleY, anchor);
}

void TextRenderer::renderText(Shader* shader, std::string text, GLfloat x, GLfloat y, GLfloat scaleX, GLfloat scaleY, UiAnchor anchor) {
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);
	scaleX *= defaultScale;
	scaleY *= defaultScale;
	std::string::const_iterator c;

	glm::vec2 size = getTextSize(text, scaleX, scaleY);
	switch (anchor) {
		default:
			throw std::exception("Unsupported UiAnchor value provided!");
		case TopLeft:
			//nothing to change
			break;
		case Top:
			x -= size.x / 2.0f;
			break;
		case TopRight:
			x -= size.x;
			break;
		case Left:
			y -= size.y / 2.0f;
			break;
		case BottomLeft:
			y -= size.y;
			break;
		case Bottom:
			x -= size.x / 2.0f;
			y -= size.y;
			break;
		case BottomRight:
			x -= size.x;
			y -= size.y;
			break;
		case Right:
			x -= size.x;
			y -= size.y / 2.0f;
			break;
		case Center:
			x -= size.x / 2.0f;
			y -= size.y / 2.0f;
	}

	for (c = text.begin(); c != text.end(); ++c) {
		Character ch = characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scaleX;
		GLfloat ypos = y + (this->characters['H'].Bearing.y - ch.Bearing.y) * scaleY;

		GLfloat w = ch.Size.x * scaleX;
		GLfloat h = ch.Size.y * scaleY;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{xpos, ypos + h, 0.0, 1.0},
			{xpos + w, ypos, 1.0, 0.0},
			{xpos, ypos, 0.0, 0.0},

			{xpos, ypos + h, 0.0, 1.0},
			{xpos + w, ypos + h, 1.0, 1.0},
			{xpos + w, ypos, 1.0, 0.0}
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		// Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph
		x += (ch.Advance >> 6) * scaleX; // Bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint TextRenderer::getFontSize() {
	return fontSize;
}

glm::vec2 TextRenderer::getScaleToFitSize(std::string text, glm::vec2 size, UiRescaleMode rescaleMode) {
	glm::vec2 actualSize = getTextSize(text);
	float multY = size.y / actualSize.y;
	float multX = size.x / actualSize.x;
	switch (rescaleMode) {
		default:
			throw std::exception("Unsupported UiRescaleMode provided!");
		case None:
		case Fit:
			glm::vec2 heightBased = getScaleToFitSize(text, size, MatchHeight);
			glm::vec2 widthBased = getScaleToFitSize(text, size, MatchWidth);
			if (heightBased.x * actualSize.x <= size.x) {
				return heightBased;
			}
			return widthBased;
		case MatchHeight:
			return glm::vec2(multY, multY);
		case MatchWidth:
			return glm::vec2(multX, multX);
		case Stretch:
			return glm::vec2(multX, multY);
	}
}

glm::vec2 TextRenderer::getTextSize(std::string text, GLfloat scale) {
	return getTextSize(text, scale, scale);
}

glm::vec2 TextRenderer::getTextSize(std::string text, GLfloat scaleX, GLfloat scaleY) {
	std::string::const_iterator c;
	float totalWidth = 0.0f, totalHeight = 0.0f;
	for (c = text.begin(); c != text.end(); ++c) {
		Character ch = characters[*c];
		totalWidth += ch.Size.x * scaleX;
		totalWidth += ch.Bearing.x * scaleX;
		float height = ch.Size.y * scaleY;
		totalHeight = totalHeight > height ? totalHeight : height;
	}
	return glm::vec2(totalWidth, totalHeight);
}

glm::vec2 TextRenderer::getTextSize(std::string text, glm::vec2 scale) {
	return getTextSize(text, scale.x, scale.y);
}
