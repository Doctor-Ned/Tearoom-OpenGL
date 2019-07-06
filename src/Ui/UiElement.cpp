#include "UiElement.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"
#include "Serialization/DataSerializer.h"

glm::vec3 UiElement::referenceRescaler = glm::vec3(1.0f, 1.0f, 0.0f);
glm::mat4 UiElement::projection = glm::mat4(1.0f);
glm::mat4 UiElement::view = lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

UiElement::UiElement(glm::vec2 position, glm::vec2 size, UiAnchor anchor) : size(size), localTransform(InterfaceTransform(dirty, size, anchor)), worldTransform(Transform(dirty)) {
	glGenVertexArrays(1, &vao);
	setRotationAnchor(Center);
	UiElement::setPosition(position, anchor);
}

void UiElement::render(Shader *shader) {
	//shader->setProjection(projection);
	CHECK_GL_ERROR();
	shader->setModel(worldTransform.getMatrix());
	shader->setOpacity(opacity);
}

void UiElement::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (active) {
		for (auto &child : children) {
			child->mouse_callback(window, xpos, ypos);
		}
	}
}

void UiElement::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (active) {
		for (auto &child : children) {
			child->mouse_button_callback(window, button, action, mods);
		}
	}
}

void UiElement::setPosition(glm::vec2 position, UiAnchor anchor) {
	this->anchor = anchor;
	glm::vec2 pos = position;
	switch (anchor) {
		default:
			SPDLOG_ERROR("Unsupported UiAnchor provided!");
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
	if (element == nullptr || element->parent != this) {
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
	if (element == nullptr) {
		return;
	}
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

void UiElement::insertChild(UiElement* element, int index) {
	if(element == nullptr) {
		return;
	}
	for(auto &child : children) {
		if(child == element) {
			return;
		}
	}
	if(element->parent != nullptr) {
		element->parent->removeChild(element);
	}
	element->parent = this;
	if(index >= children.size() || index < 0) {
		children.push_back(element);
	} else {
		children.insert(children.begin() + index, element);
	}
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

void UiElement::updateProjection() {
	GameManager *gm = GameManager::getInstance();
	referenceRescaler = glm::vec3((gm->getWindowWidth() / UI_REF_WIDTH) * (gm->getScreenWidth() / gm->getWindowWidth()), (gm->getWindowHeight() / UI_REF_HEIGHT) * (gm->getScreenHeight() / gm->getWindowHeight()), 0.0f);
	//referenceRescaler = glm::vec3(UI_REF_WIDTH / GameManager::getInstance()->getWindowWidth(), UI_REF_HEIGHT / GameManager::getInstance()->getWindowHeight(), 0.0f);
	projection = glm::ortho(0.0f, static_cast<GLfloat>(UI_REF_WIDTH), static_cast<GLfloat>(UI_REF_HEIGHT), 0.0f);
	for (auto &pair : AssetManager::getInstance()->getShaders()) {
		switch (pair.first) {
			case STUiTexture:
			case STUiColor:
			case STUiText:
				pair.second->use();
				pair.second->setView(view);
				pair.second->setProjection(projection);
				break;
		}
	}
}

UiElement::~UiElement() {
	SPDLOG_DEBUG("UI_Element destructor called");
	glDeleteVertexArrays(1, &vao);
	if (vbo != 0) {
		glDeleteBuffers(1, &vbo);
	}
	for (UiElement* element : children)
	{
		delete element;
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
			worldTransformWithoutRotationScale = translate(parent->worldTransform.getMatrix(), localTransform.getPosition());
		} else {
			worldTransform.setMatrix(localTransform.getMatrix());
			worldTransformWithoutRotationScale = translate(glm::mat4(1.0f), localTransform.getPosition());
		}
		//modeledPosition = glm::vec2(worldTransform.getMatrix()[3]);
		modeledPosition = glm::vec2(worldTransformWithoutRotationScale[3]);
		glm::vec3 scale = Global::getScale(worldTransform.getMatrix());
		modeledSize = size;
		modeledSize.x *= scale.x;
		modeledSize.y *= scale.y;
	}
}

glm::vec2 UiElement::createScaledSize(float width, float height) {
	return glm::vec2(UI_REF_WIDTH*width, UI_REF_HEIGHT*height);
}

glm::vec2 UiElement::createSizeScaledByWidth(float size) {
	float value = UI_REF_WIDTH * size;
	return glm::vec2(value, value);
}

glm::vec2 UiElement::createSizeScaledByHeight(float size) {
	float value = UI_REF_HEIGHT * size;
	return glm::vec2(value, value);
}

glm::vec2 UiElement::getRescaledSize() {
	GameManager *gm = GameManager::getInstance();
	return glm::vec2(size.x * (gm->getScreenWidth() / gm->getWindowWidth()), size.y * (gm->getScreenHeight() / gm->getWindowHeight()));
}

glm::vec2 UiElement::getRescaledPosition() {
	GameManager *gm = GameManager::getInstance();
	return glm::vec2(localTransform.getPosition().x *(gm->getScreenWidth() / gm->getWindowWidth()), localTransform.getPosition().y * (gm->getScreenHeight() / gm->getWindowHeight()));
}

glm::vec2 UiElement::getRescaledModeledSize() {
	GameManager *gm = GameManager::getInstance();
	return glm::vec2(modeledSize.x * referenceRescaler.x, modeledSize.y * referenceRescaler.y);
}

glm::vec2 UiElement::getRescaledModeledPosition() {
	GameManager *gm = GameManager::getInstance();
	return glm::vec2(modeledPosition.x * referenceRescaler.x, modeledPosition.y * referenceRescaler.y);
}

void UiElement::setup() {}
