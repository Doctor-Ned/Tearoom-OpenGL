#include "UiElement.h"
#include "Scene/SceneManager.h"

UiElement::UiElement(const char* texture, glm::vec2 position, glm::vec2 size, bool center) {
	if (texture != nullptr) {
		this->texture = createTexture(texture);
	}
	SceneManager *manager = SceneManager::getInstance();
	this->size = size;
	this->center = center;
	glGenVertexArrays(1, &vao);
	updateProjection(manager->getWindowWidth(), manager->getWindowHeight(), manager->getScreenWidth(), manager->getScreenHeight());
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

void UiElement::updateProjection(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), static_cast<GLfloat>(windowHeight), 0.0f);
}

UiElement::~UiElement() {
	glDeleteVertexArrays(1, &vao);
	if (vbo != 0) {
		glDeleteBuffers(1, &vbo);
	}
}

ShaderType UiElement::getShaderType() {
	return STNone;
}

glm::vec2 UiElement::getRescaledPosition() {
	return glm::vec2(actualPosition.x * screenWidth / windowWidth, actualPosition.y * screenHeight / windowHeight);
}

glm::vec2 UiElement::getRescaledSize() {
	return glm::vec2(size.x * screenWidth / windowWidth, size.y * screenHeight / windowHeight);
}
