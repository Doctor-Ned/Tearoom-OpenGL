#pragma once

#include "Global.h"
#include "UiElement.h"

class UiTexturedElement : public UiElement {
protected:
	UiTexturedElement(const char* texture, glm::vec2 position, glm::vec2 size, bool center = true);
	Texture texture;
};
