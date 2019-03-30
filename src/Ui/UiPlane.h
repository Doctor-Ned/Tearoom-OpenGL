#ifndef UIPLANE_H
#define UIPLANE_H
#include "UiElement.h"

class UiPlane : public UiElement {
public:
	UiPlane(Shader* shader, const char* texture, glm::vec2 position, glm::vec2 size, bool center = true);
	void render() override;
	void setup() override;
};

#endif
