#include "InterfaceTransform.h"
#include <glm/gtx/euler_angles.inl>
InterfaceTransform::InterfaceTransform(bool& dirty, glm::vec2 size, UiAnchor anchor) : ComposedTransform(dirty), size(size), anchor(anchor) {}

glm::mat4 InterfaceTransform::dataToMatrix(TransformData data) {
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), data.translation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), data.scale);
	glm::vec2 size = glm::vec2(scale * glm::vec4(this->size, 0.0f, 1.0f));
	glm::vec2 rotTranslation;
	switch (anchor) {
		case TopLeft:
			rotTranslation = glm::vec2(0.0f, 0.0f);
			break;
		case Top:
			rotTranslation = glm::vec2(-size.x / 2.0f, 0.0f);
			break;
		case TopRight:
			rotTranslation = glm::vec2(-size.x);
			break;
		case Left:
			rotTranslation = glm::vec2(0.0f, -size.y / 2.0f);
			break;
		case BottomLeft:
			rotTranslation = glm::vec2(0.0f, -size.y);
			break;
		case Bottom:
			rotTranslation = glm::vec2(-size.x / 2.0f, -size.y);
			break;
		case BottomRight:
			rotTranslation = glm::vec2(-size.x, -size.y);
			break;
		case Right:
			rotTranslation = glm::vec2(-size.x, -size.y / 2.0f);
			break;
		case Center:
			rotTranslation = glm::vec2(-size.x / 2.0f, -size.y / 2.0f);
			break;
	}
	rotTranslation -= position;
	glm::mat4 rotTrans = glm::translate(glm::mat4(1.0f), glm::vec3(rotTranslation, 0.0f));
	glm::mat4 invRotTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-rotTranslation, 0.0f));
	glm::mat4 rotation = glm::eulerAngleYXZ(data.eulerRotation.y, data.eulerRotation.x, data.eulerRotation.z);
	return translation * invRotTrans * rotation * rotTrans * scale;
}

void InterfaceTransform::updateSize(glm::vec2 size) {
	this->size = size;
}

void InterfaceTransform::updateAnchor(UiAnchor anchor) {
	this->anchor = anchor;
}

void InterfaceTransform::updatePosition(glm::vec2 position) {
	this->position = position;
}
