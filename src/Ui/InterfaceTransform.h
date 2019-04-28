#pragma once

#include "Scene/ComposedTransform.h"
enum UiAnchor;

class InterfaceTransform : public ComposedTransform {
public:
	InterfaceTransform(bool &dirty, glm::vec2 size, UiAnchor anchor);
	glm::mat4 dataToMatrix(TransformData data) override;
	void updateSize(glm::vec2 size);
	void updateAnchor(UiAnchor anchor);
	void updatePosition(glm::vec2 position);
protected:
	glm::vec2 position = glm::vec2(0.0f, 0.0f);
	glm::vec2 size;
	UiAnchor anchor;
};