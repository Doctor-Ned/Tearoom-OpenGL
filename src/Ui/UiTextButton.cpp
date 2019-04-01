#include "UiTextButton.h"

UiTextButton::UiTextButton(const char* textureIdle, const char* textureHover, const char* textureClicked,
	glm::vec2 position, glm::vec2 size, TextRenderer* textRenderer, const char* text,
	glm::vec3 textColor, bool center) : UiButton(textureIdle, textureHover, textureClicked, position, size, center) {
	this->textRenderer = textRenderer;
	this->text = text;
	this->color = textColor;
	setup();
}

UiTextButton::UiTextButton(glm::vec2 position, TextRenderer* textRenderer, const char* text, glm::vec3 textColor,
	bool center) : UiTextButton(BTN_LONG_IDLE, BTN_LONG_HOVER, BTN_LONG_CLICKED, position, createScaledSize(BASE_LONG_BTN_WIDTH, BASE_LONG_BTN_HEIGHT), textRenderer, text, textColor, center) {}

void UiTextButton::render() {
	UiButton::render();
	textRenderer->renderText(text, textAnchor.x, textAnchor.y, 1.0f, true, color);
}

void UiTextButton::setText(char* text) {
	this->text = text;
}

void UiTextButton::setPosition(glm::vec2 position, bool center) {
	UiElement::setPosition(position, center);
	textAnchor = glm::vec2(actualPosition.x + size.x / 2.0f, actualPosition.y + size.y / 2.0f);
}

void UiTextButton::setup() {}
