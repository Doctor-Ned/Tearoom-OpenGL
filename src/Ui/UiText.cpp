#include "UiText.h"

UiText::UiText(glm::vec2 position, glm::vec2 size, const char* text, glm::vec3 textColor, UiRescaleMode rescaleMode,
	bool center, TextRenderer* textRenderer) : UiElement(position, size, center) {
	this->color = textColor;
	this->text = text;
	this->textRenderer = textRenderer;
	this->rescaleMode = rescaleMode;
	scale = textRenderer->getScaleToFitSize(text, size, rescaleMode);
	UiText::setPosition(position, center);
}

void UiText::render() {
	textRenderer->renderText(text, actualPosition, scale, center, color);
}

void UiText::setText(const char * text) {
	this->text = text;
}

void UiText::setup() {}

void UiText::setPosition(glm::vec2 position, bool center) {
	this->center = center;
	actualPosition = position;
}

ShaderType UiText::getShaderType() {
	return STNone;
}
