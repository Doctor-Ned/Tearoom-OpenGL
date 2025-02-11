#include "UiColorPlane.h"
#include "Scene/AssetManager.h"

UiColorPlane::
UiColorPlane(glm::vec4 color, glm::vec2 position, glm::vec2 size, UiAnchor anchor) : UiElement(position, size, anchor) {
	this->color = color;
	setup();
}

void UiColorPlane::render(Shader *shader) {
	CHECK_GL_ERROR();
	UiElement::render(shader);
	CHECK_GL_ERROR();
	shader->setColor(color);
	glBindVertexArray(vao);
	CHECK_GL_ERROR();
	glBindVertexBuffer(0, vbo, 0, sizeof(UiVertex));
	CHECK_GL_ERROR();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	CHECK_GL_ERROR();
	glBindVertexArray(0);
	CHECK_GL_ERROR();
	
}

void UiColorPlane::setup() {

	UiVertex vertices[4];

	vertices[0].Position = glm::vec2(0.0f, size.y);
	vertices[1].Position = glm::vec2(0.0f, 0.0f);
	vertices[2].Position = glm::vec2(size.x, 0.0f);
	vertices[3].Position = glm::vec2(size.x, size.y);

	std::vector<UiVertex> data;
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
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(UiVertex), &data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UiVertex), static_cast<void*>(nullptr));

	glBindVertexArray(0);
	data.clear();
}

ShaderType UiColorPlane::getShaderType() {
	return STUiColor;
}
