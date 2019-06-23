#include "UiTexturedElement.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"

QuadData UiTexturedElement::createFullscreenTexturedQuad() {
	CHECK_GL_ERROR();
	QuadData result;

	UiTextureVertex vertices[4];
	vertices[0].TexCoords = glm::vec2(0.0f, 1.0f);
	vertices[1].TexCoords = glm::vec2(0.0f, 0.0f);
	vertices[2].TexCoords = glm::vec2(1.0f, 0.0f);
	vertices[3].TexCoords = glm::vec2(1.0f, 1.0f);

	vertices[0].Position = glm::vec2(-1.0f, 1.0f);
	vertices[1].Position = glm::vec2(-1.0f, -1.0f);
	vertices[2].Position = glm::vec2(1.0f, -1.0f);
	vertices[3].Position = glm::vec2(1.0f, 1.0f);

	std::vector<UiTextureVertex> data;
	data.push_back(vertices[0]);
	data.push_back(vertices[1]);
	data.push_back(vertices[2]);
	data.push_back(vertices[0]);
	data.push_back(vertices[2]);
	data.push_back(vertices[3]);

	glGenVertexArrays(1, &result.vao);
	glGenBuffers(1, &result.vbo);
	glBindVertexArray(result.vao);
	glBindBuffer(GL_ARRAY_BUFFER, result.vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(UiTextureVertex), &data[0], GL_STATIC_DRAW);
	CHECK_GL_ERROR();

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UiTextureVertex), static_cast<void*>(nullptr));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UiTextureVertex),
						  reinterpret_cast<void*>(offsetof(UiTextureVertex, TexCoords)));

	CHECK_GL_ERROR();
	data.clear();
	glBindVertexArray(0);
	CHECK_GL_ERROR();
	return result;
}

UiTexturedElement::UiTexturedElement(const char* texture, glm::vec2 position, glm::vec2 size, UiAnchor anchor) : UiElement(position, size, anchor) {
	if (texture != nullptr) {
		this->texture = AssetManager::getInstance()->getTexture(texture);
	}
}
