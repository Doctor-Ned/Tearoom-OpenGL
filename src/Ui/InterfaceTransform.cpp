#include "Global.h"
#include "InterfaceTransform.h"
#include <glm/gtx/euler_angles.inl>
#include "UiElement.h"

InterfaceTransform::InterfaceTransform(bool& dirty, glm::vec2 size, UiAnchor anchor) : ComposedTransform(dirty), size(size), anchor(anchor) {}

glm::mat4 InterfaceTransform::dataToMatrix(const TransformData& data) {
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), data.translation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), data.scale);
	//glm::vec2 size = glm::vec2(scale * glm::vec4(this->size, 0.0f, 1.0f));
	glm::vec2 rotTranslation = glm::vec2(0.0f, 0.0f);
	switch (anchor) {
		case TopLeft:
			break;
		case Top:
			rotTranslation = glm::vec2(-this->size.x / 2.0f, 0.0f);
			break;
		case TopRight:
			rotTranslation = glm::vec2(-this->size.x, 0.0f);
			break;
		case Left:
			rotTranslation = glm::vec2(0.0f, -this->size.y / 2.0f);
			break;
		case BottomLeft:
			rotTranslation = glm::vec2(0.0f, -this->size.y);
			break;
		case Bottom:
			rotTranslation = glm::vec2(-this->size.x / 2.0f, -this->size.y);
			break;
		case BottomRight:
			rotTranslation = glm::vec2(-this->size.x, -this->size.y);
			break;
		case Right:
			rotTranslation = glm::vec2(-this->size.x, -this->size.y / 2.0f);
			break;
		case Center:
			rotTranslation = glm::vec2(-this->size.x / 2.0f, -this->size.y / 2.0f);
			break;
	}
	rotTranslation -= glm::vec2(data.translation.x, data.translation.y);
	glm::mat4 rotTrans = glm::translate(glm::mat4(1.0f), glm::vec3(rotTranslation, 0.0f));
	glm::mat4 invRotTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-rotTranslation, 0.0f));
	glm::mat4 rotation = glm::eulerAngleYXZ(data.eulerRotation.y, data.eulerRotation.x, data.eulerRotation.z);
	return invRotTrans * rotation * scale * rotTrans * translation;
}

void InterfaceTransform::updateSize(glm::vec2 size) {
	this->size = size;
	*dirty = true;
	selfDirty = true;
}

void InterfaceTransform::updateAnchor(UiAnchor anchor) {
	this->anchor = anchor;
	*dirty = true;
	selfDirty = true;
}
