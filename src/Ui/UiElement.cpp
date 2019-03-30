#include "UiElement.h"

UiElement::UiElement(Shader* shader, const char* texture, glm::vec2 position, glm::vec2 size, bool center) {
	if (texture != nullptr) {
		this->texture = createTexture(texture);
	}
	this->shader = shader;
	this->size = size;
	this->center = center;
	glGenVertexArrays(1, &vao);
	reworkProjection();
	setPosition(position);
}

void UiElement::render() {
	shader->use();
	shader->setProjection(projection);
}

void UiElement::setPosition(glm::vec2 position, bool center) {
	if (center) {
		actualPosition = glm::vec2(position.x - size.x / 2.0f, position.y - size.y / 2.0f);
	} else {
		actualPosition = position;
	}
}

glm::vec2 UiElement::getPosition() {
	return actualPosition;
}

glm::vec2 UiElement::getCenter() {
	return glm::vec2(actualPosition.x + size.x / 2.0f, actualPosition.y + size.y / 2.0f);
}

glm::vec2 UiElement::getSize() {
	return size;
}

void UiElement::reworkProjection() {
	projection = glm::ortho(0.0f, static_cast<GLfloat>(WINDOW_WIDTH), static_cast<GLfloat>(WINDOW_HEIGHT), 0.0f);
}

UiElement::~UiElement() {
	glDeleteVertexArrays(1, &vao);
	if(vbo != 0) {
		glDeleteBuffers(1, &vbo);
	}
}
