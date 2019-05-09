#include "UiElement.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"
#include "Serialization/DataSerializer.h"

float UiElement::windowWidth = 0.0f;
float UiElement::windowHeight = 0.0f;
float UiElement::screenWidth = 0.0f;
float UiElement::screenHeight = 0.0f;
glm::mat4 UiElement::projection = glm::mat4(1.0f);
glm::mat4 UiElement::view = lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

UiElement::UiElement(glm::vec2 position, glm::vec2 size, UiAnchor anchor) : size(size), rotationAnchor(Center), localTransform(InterfaceTransform(dirty, size, anchor)), worldTransform(Transform(dirty)) {
	glGenVertexArrays(1, &vao);
	UiElement::setPosition(position, anchor);
}

void UiElement::render(Shader *shader) {
	//shader->setProjection(projection);
	shader->setModel(worldTransform.getMatrix());
	shader->setOpacity(opacity);
}

void UiElement::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	for (auto &child : children) {
		child->mouse_callback(window, xpos, ypos);
	}
}

void UiElement::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	for (auto &child : children) {
		child->mouse_button_callback(window, button, action, mods);
	}
}

void UiElement::setPosition(glm::vec2 position, UiAnchor anchor) {
	this->anchor = anchor;
	glm::vec2 pos = position;
	switch (anchor) {
		default:
			throw std::exception("Unsupported UiAnchor provided!");
		case TopLeft:
		//already set	
			break;
		case Top:
			pos.x -= size.x / 2.0f;
			break;
		case TopRight:
			pos.x -= size.x;
			break;
		case Left:
			pos.y -= size.y / 2.0f;
			break;
		case BottomLeft:
			pos.y -= size.y;
			break;
		case Bottom:
			pos.x -= size.x / 2.0f;
			pos.y -= size.y;
			break;
		case BottomRight:
			pos.x -= size.x;
			pos.y -= size.y;
			break;
		case Right:
			pos.x -= size.x;
			pos.y -= size.y / 2.0f;
			break;
		case Center:
			pos.x -= size.x / 2.0f;
			pos.y -= size.y / 2.0f;
			break;
	}
	localTransform.setPosition(pos.x, pos.y, localTransform.getMatrix()[3].z);
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

glm::vec2 UiElement::getPosition() {
	return glm::vec2(localTransform.getMatrix()[3]);
}

glm::vec2 UiElement::getModeledPosition() {
	return modeledPosition;
}

glm::vec2 UiElement::getModeledCenter() {
	return modeledPosition + modeledSize / 2.0f;
}

glm::vec2 UiElement::getCenter() {
	return glm::vec2(localTransform.getMatrix()[3].x + size.x / 2.0f, localTransform.getMatrix()[3].y + size.y / 2.0f);
}

glm::vec2 UiElement::getSize() const {
	return size;
}

void UiElement::removeChild(UiElement* element) {
	if (element->parent != this) {
		return;
	}
	for (auto i = children.begin(); i != children.end();) {
		if (*i == element) {
			children.erase(i);
			element->parent = nullptr;
			break;
		}
		++i;
	}
}

void UiElement::setParent(UiElement* element) {
	if (parent != nullptr) {
		parent->removeChild(this);
	}
	element->addChild(this);
}

void UiElement::addChild(UiElement* element) {
	for (auto &child : children) {
		if (child == element) {
			return;
		}
	}
	if (element->parent != nullptr) {
		element->parent->removeChild(element);
	}
	element->parent = this;
	children.push_back(element);
}

UiElement* UiElement::getChild(int index) {
	if (index >= 0 && index < children.size()) {
		return children[index];
	}
	return nullptr;
}

std::vector<UiElement*> UiElement::getChildren() {
	return children;
}

void UiElement::updateDrawData() {
	if (!active) {
		return;
	}

	if (parent != nullptr) {
		dirty |= parent->dirty;
		if (dirty) {
			updateWorld();
		}
	} else if (dirty) {
		updateWorld();
	}

	for (auto &child : children) {
		child->updateDrawData();
	}

	dirty = false;
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
				pair.second->setView(view);
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

void UiElement::setActive(bool active) {
	this->active = active;
}

bool UiElement::isActive() {
	return active;
}

void UiElement::setRotationAnchor(UiAnchor anchor) {
	this->rotationAnchor = anchor;
	localTransform.updateAnchor(anchor);
}

void UiElement::updateWorld() {
	if (dirty) {
		if (parent != nullptr) {
			worldTransform.setMatrix(parent->worldTransform.getMatrix()* localTransform.getMatrix());
		} else {
			worldTransform.setMatrix(localTransform.getMatrix());
		}
		modeledPosition = glm::vec2(worldTransform.getMatrix()[3]);
		glm::vec3 scale = Global::getScale(worldTransform.getMatrix());
		modeledSize = size;
		modeledSize.x *= scale.x;
		modeledSize.y *= scale.y;
	}
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
	return glm::vec2(localTransform.getMatrix()[3].x * screenWidth / windowWidth, localTransform.getMatrix()[3].y * screenHeight / windowHeight);
}

glm::vec2 UiElement::getRescaledSize() {
	return glm::vec2(size.x * screenWidth / windowWidth, size.y * screenHeight / windowHeight);
}

glm::vec2 UiElement::getRescaledModeledPosition() {
	return glm::vec2(modeledPosition.x * screenWidth / windowWidth, modeledPosition.y * screenHeight / windowHeight);
}

glm::vec2 UiElement::getRescaledModeledSize() {
	return glm::vec2(modeledSize.x * screenWidth / windowWidth, modeledSize.y * screenHeight / windowHeight);
}

void UiElement::setup() {}
