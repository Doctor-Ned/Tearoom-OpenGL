#pragma once

#include "Scene/ComposedTransform.h"
enum UiAnchor;

class InterfaceTransform : public ComposedTransform {
public:
	InterfaceTransform(bool &dirty, glm::vec2 size, UiAnchor anchor);
	glm::mat4 dataToMatrix(const TransformData& data) override;
	void updateSize(glm::vec2 size);
	void updateAnchor(UiAnchor anchor);
protected:
	glm::vec2 size;
	UiAnchor anchor;
};