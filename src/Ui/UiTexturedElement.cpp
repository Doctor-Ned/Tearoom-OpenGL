#include "UiTexturedElement.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"

UiTexturedElement::UiTexturedElement(const char* texture, glm::vec2 position, glm::vec2 size, bool center) : UiElement(position, size, center) {
	if (texture != nullptr) {
		this->texture = AssetManager::getInstance()->getTexture(texture);
	}
}