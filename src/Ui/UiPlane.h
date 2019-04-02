#ifndef UIPLANE_H
#define UIPLANE_H
#include "UiTexturedElement.h"

class UiPlane : public UiTexturedElement {
public:
	UiPlane(const char* texture, glm::vec2 position, glm::vec2 size, bool center = true);
	void render() override;
	void setup() override;
	ShaderType getShaderType() override;
};

#endif
