#include "Scene/AssetManager.h"
#include "UiPlane.h"

UiPlane::UiPlane(const char* texture, glm::vec2 position, glm::vec2 size, UiAnchor anchor) : UiTexturedElement(texture, position, size, anchor) {
	setup();
}

void UiPlane::setTexture(const char* texture) {
	AssetManager::getInstance()->getTexture(texture);
}

void UiPlane::render(Shader *shader) {
	CHECK_GL_ERROR();
	UiTexturedElement::render(shader);
	CHECK_GL_ERROR();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glBindVertexArray(vao);
	CHECK_GL_ERROR();
	glBindVertexBuffer(0, vbo, 0, sizeof(UiTextureVertex));
	CHECK_GL_ERROR();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	CHECK_GL_ERROR();
	glBindVertexArray(0);
	CHECK_GL_ERROR();
}

void UiPlane::setup() {
	UiTextureVertex vertices[4];

	vertices[0].TexCoords = glm::vec2(0.0f, 1.0f);
	vertices[1].TexCoords = glm::vec2(0.0f, 0.0f);
	vertices[2].TexCoords = glm::vec2(1.0f, 0.0f);
	vertices[3].TexCoords = glm::vec2(1.0f, 1.0f);

	vertices[0].Position = glm::vec2(0.0f, size.y);
	vertices[1].Position = glm::vec2(0.0f, 0.0f);
	vertices[2].Position = glm::vec2(size.x, 0.0f);
	vertices[3].Position = glm::vec2(size.x, 0.0f + size.y);

	std::vector<UiTextureVertex> data;
	data.push_back(vertices[0]);
	data.push_back(vertices[2]);
	data.push_back(vertices[1]);
	data.push_back(vertices[0]);
	data.push_back(vertices[3]);
	data.push_back(vertices[2]);

	if(vbo != 0) {
		glDeleteBuffers(1, &vbo);
	}

	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(UiTextureVertex), &data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UiTextureVertex), static_cast<void*>(nullptr));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UiTextureVertex), reinterpret_cast<void*>(offsetof(UiTextureVertex, TexCoords)));

	glBindVertexArray(0);
	data.clear();
}

ShaderType UiPlane::getShaderType() {
	return STUiTexture;
}
