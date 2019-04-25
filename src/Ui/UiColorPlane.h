#ifndef UICOLORPLANE_H
#define UICOLORPLANE_H
#include "UiElement.h"

class UiColorPlane : public UiElement {
public:
	UiColorPlane(glm::vec4 color, glm::vec2 position, glm::vec2 size, UiAnchor anchor = Center);
	void render(Shader *shader) override;
	void setup() override;
	ShaderType getShaderType() override;
protected:
	glm::vec4 color;
};

#endif
