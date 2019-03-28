#include "UiTextButton.h"

UiTextButton::UiTextButton(Shader* shader, const char* textureIdle, const char* textureHover,
                           const char* textureClicked,
                           glm::vec2 position, glm::vec2 size, TextRenderer* textRenderer, char* text,
                           glm::vec3 textColor, bool center) : UiButton(shader, textureIdle, textureHover,
                                                                        textureClicked, position, size, center) {
	this->textRenderer = textRenderer;
	this->text = text;
	this->color = textColor;
	textAnchor = glm::vec2(actualPosition.x + size.x / 2.0f, actualPosition.y + size.y / 2.0f);
}

void UiTextButton::render() {
	UiButton::render();
	textRenderer->renderText(text, textAnchor.x, textAnchor.y, 1.0f, true, color);
}

void UiTextButton::setText(char* text) {
	this->text = text;
}
