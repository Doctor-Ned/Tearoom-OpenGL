#pragma once

#include "Global.h"
#include "UiElement.h"

struct QuadData {
	GLuint vao = 0;
	GLuint vbo = 0;
	void dispose() {
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		vbo = 0;
		vao = 0;
	}

	void render() const {
		glBindVertexArray(vao);
		glBindVertexBuffer(0, vbo, 0, sizeof(UiTextureVertex));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
};

class UiTexturedElement : public UiElement {
public:
	static QuadData createFullscreenTexturedQuad();
protected:
	UiTexturedElement(const char* texture, glm::vec2 position, glm::vec2 size, UiAnchor anchor = Center);
	Texture texture;
};
