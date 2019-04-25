#pragma once
#include "UiElement.h"

class UiCanvas : public UiElement {
public:
	UiCanvas(glm::vec2 position, glm::vec2 size, UiAnchor anchor = TopLeft);
	ShaderType getShaderType() override;
	void setup() override;
};