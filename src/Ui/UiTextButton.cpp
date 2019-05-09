#include "UiTextButton.h"

UiTextButton::UiTextButton(const char* textureIdle, const char* textureHover, const char* textureClicked,
	glm::vec2 position, glm::vec2 size, const char* text, float textHeightScale,
	glm::vec3 textColor, UiAnchor anchor, TextRenderer* textRenderer) : UiButton(textureIdle, textureHover, textureClicked, position, size, anchor) {
	this->text = new UiText(position, size * textHeightScale, text, textColor, MatchHeight, Center, textRenderer);
	addChild(this->text);
	UiTextButton::setPosition(position, anchor);
	setup();
}

UiTextButton::UiTextButton(glm::vec2 position, const char* text, glm::vec3 textColor,
	UiAnchor anchor, TextRenderer* textRenderer) : UiTextButton(position, createScaledSize(BASE_LONG_BTN_WIDTH, BASE_LONG_BTN_HEIGHT), text, textColor, anchor, textRenderer) {}

UiTextButton::UiTextButton(glm::vec2 position, glm::vec2 size, const char* text, glm::vec3 textColor,
	UiAnchor anchor, TextRenderer* textRenderer) : UiTextButton(BTN_LONG_IDLE, BTN_LONG_HOVER, BTN_LONG_CLICKED,
		position, size, text, BASE_TEXT_HEIGHT_SCALE, textColor, anchor, textRenderer) {}

void UiTextButton::setText(char* text) {
	this->text->setText(text);
}

void UiTextButton::setPosition(glm::vec2 position, UiAnchor anchor) {
	UiElement::setPosition(position, anchor);
	this->text->setPosition(glm::vec2(size.x / 2.0f, size.y / 2.0f), Center);
}
