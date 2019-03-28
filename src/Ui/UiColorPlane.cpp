#include "UiColorPlane.h"

UiColorPlane::
UiColorPlane(Shader* shader, glm::vec4 color, glm::vec2 position, glm::vec2 size, bool center) : UiElement(
	shader, nullptr, position, size, center) {
	this->color = color;
	const float minX = actualPosition.x;
	const float minY = actualPosition.y;

	UiVertex vertices[4];

	vertices[0].Position = glm::vec2(minX, minY + size.y);
	vertices[1].Position = glm::vec2(minX, minY);
	vertices[2].Position = glm::vec2(minX + size.x, minY);
	vertices[3].Position = glm::vec2(minX + size.x, minY + size.y);

	std::vector<UiVertex> data;
	data.push_back(vertices[0]);
	data.push_back(vertices[2]);
	data.push_back(vertices[1]);
	data.push_back(vertices[0]);
	data.push_back(vertices[3]);
	data.push_back(vertices[2]);

	shader->use();

	glGenVertexArrays(1, &vao);

	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(UiVertex), &data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UiVertex), (void*)nullptr);

	glBindVertexArray(0);
	data.clear();
}

void UiColorPlane::render() {
	UiElement::render();
	shader->setColor(color);
	glBindVertexArray(vao);
	glBindVertexBuffer(0, vbo, 0, sizeof(UiVertex));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
}

void UiColorPlane::mouse_callback(GLFWwindow* window, double xpos, double ypos) {}

void UiColorPlane::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {}
