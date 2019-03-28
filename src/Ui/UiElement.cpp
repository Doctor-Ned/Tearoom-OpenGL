#include "UiElement.h"

UiElement::UiElement(Shader* shader, const char* texture, glm::vec2 position, glm::vec2 size, bool center) {
	if (texture != nullptr) {
		this->texture = createTexture(texture);
	}
	this->shader = shader;
	this->position = position;
	this->size = size;
	this->center = center;
	if (center) {
		actualPosition = glm::vec2(position.x - size.x / 2.0f, position.y - size.y / 2.0f);
	}
	else {
		actualPosition = position;
	}
}

void UiElement::render() {
	shader->use();
	shader->setProjection(projection);
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
