#ifndef UIPLANE_H
#define UIPLANE_H
#include "UiTexturedElement.h"

class UiPlane : public UiTexturedElement {
public:
	UiPlane(const char* texture, glm::vec2 position, glm::vec2 size, UiAnchor anchor);
	void render(Shader *shader) override;
	void setup() override;
	ShaderType getShaderType() override;
};

#endif
