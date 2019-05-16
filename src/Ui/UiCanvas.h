#pragma once
#include "UiElement.h"

class UiCanvas : public UiElement {
public:
	UiCanvas(glm::vec2 position, glm::vec2 size, UiAnchor anchor = TopLeft);
	void updateWorld() override;
	ShaderType getShaderType() override;
	void setRoot();
private:
	bool isRoot = false;
};