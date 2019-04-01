#include "UiTexturedElement.h"
#include "Scene/SceneManager.h"

UiTexturedElement::UiTexturedElement(const char* texture, glm::vec2 position, glm::vec2 size, bool center) : UiElement(position, size, center) {
	if (texture != nullptr) {
		this->texture = Global::createTexture(texture);
	}
}