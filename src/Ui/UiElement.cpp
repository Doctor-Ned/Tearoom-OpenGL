#include "UiElement.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"
#include "Serialization/DataSerializer.h"

float UiElement::windowWidth = 0.0f;
float UiElement::windowHeight = 0.0f;
float UiElement::screenWidth = 0.0f;
float UiElement::screenHeight = 0.0f;
glm::mat4 UiElement::projection = glm::mat4(1.0f);

UiElement::UiElement(glm::vec2 position, glm::vec2 size, UiAnchor anchor) : size(size), localTransform(Transform(dirty)), worldTransform(Transform(dirty)) {
	glGenVertexArrays(1, &vao);
	UiElement::setPosition(position, anchor);
}

void UiElement::render(Shader *shader) {
	//shader->setProjection(projection);
	shader->setOpacity(opacity);
}

void UiElement::setPosition(glm::vec2 position, UiAnchor anchor) {
	this->anchor = anchor;
	actualPosition = position;
	switch (anchor) {
		default:
			throw std::exception("Unsupported UiAnchor provided!");
		case TopLeft:
		//already set	
		break;
		case Top:
			actualPosition.x -= size.x / 2.0f;
			break;
		case TopRight:
			actualPosition.x -= size.x;
			break;
		case Left:
			actualPosition.y -= size.y / 2.0f;
			break;
		case BottomLeft:
			actualPosition.y -= size.y;
			break;
		case Bottom:
			actualPosition.x -= size.x / 2.0f;
			actualPosition.y -= size.y;
			break;
		case BottomRight:
			actualPosition.x -= size.x;
			actualPosition.y -= size.y;
			break;
		case Right:
			actualPosition.x -= size.x;
			actualPosition.y -= size.y / 2.0f;
			break;
		case Center:
			actualPosition.x -= size.x / 2.0f;
			actualPosition.y -= size.y / 2.0f;
			break;
	}
}

void UiElement::setPosition(glm::vec2 position) {
	setPosition(position, anchor);
}

float UiElement::getOpacity() const {
	return opacity;
}

void UiElement::setOpacity(float opacity) {
	this->opacity = opacity;
}

glm::vec2 UiElement::getPosition() const {
	return actualPosition;
}

glm::vec2 UiElement::getCenter() const {
	return glm::vec2(actualPosition.x + size.x / 2.0f, actualPosition.y + size.y / 2.0f);
}

glm::vec2 UiElement::getSize() const {
	return size;
}

void UiElement::updateProjection(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	UiElement::windowWidth = windowWidth;
	UiElement::windowHeight = windowHeight;
	UiElement::screenWidth = screenWidth;
	UiElement::screenHeight = screenHeight;

	projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), static_cast<GLfloat>(windowHeight), 0.0f);
	for (auto &pair : AssetManager::getInstance()->getShaders()) {
		switch (pair.first) {
			case STUiTexture:
			case STUiColor:
			case STText:
				pair.second->use();
				pair.second->setProjection(projection);
				break;
		}
	}
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

glm::mat4 UiElement::getProjection() {
	return projection;
}

glm::vec2 UiElement::createScaledSize(float width, float height) {
	return glm::vec2(GameManager::getInstance()->getWindowWidth()*width, GameManager::getInstance()->getWindowHeight()*height);
}

glm::vec2 UiElement::createSizeScaledByWidth(float size) {
	float value = GameManager::getInstance()->getWindowWidth()*size;
	return glm::vec2(value, value);
}

glm::vec2 UiElement::createSizeScaledByHeight(float size) {
	float value = GameManager::getInstance()->getWindowHeight()*size;
	return glm::vec2(value, value);
}

glm::vec2 UiElement::getRescaledPosition() {
	return glm::vec2(actualPosition.x * screenWidth / windowWidth, actualPosition.y * screenHeight / windowHeight);
}

glm::vec2 UiElement::getRescaledSize() {
	return glm::vec2(size.x * screenWidth / windowWidth, size.y * screenHeight / windowHeight);
}
