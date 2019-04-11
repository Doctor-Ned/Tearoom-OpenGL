#include "UiText.h"

UiText::UiText(glm::vec2 position, glm::vec2 size, const char* text, glm::vec3 textColor, UiRescaleMode rescaleMode,
	bool center, TextRenderer* textRenderer) : UiElement(position, size, center) {
	this->color = textColor;
	this->textRenderer = textRenderer;
	this->rescaleMode = rescaleMode;
	this->text = text;
	scale = textRenderer->getScaleToFitSize(text, size, rescaleMode);
	UiText::setPosition(position, center);
}

UiText::UiText(glm::vec2 position, glm::vec2 size, std::string text, glm::vec3 textColor, UiRescaleMode rescaleMode,
	bool center, TextRenderer* textRenderer) : UiElement(position, size, center) {
	this->color = textColor;
	this->textRenderer = textRenderer;
	this->rescaleMode = rescaleMode;
	this->text = text;
	scale = textRenderer->getScaleToFitSize(text, size, rescaleMode);
	UiText::setPosition(position, center);
}

UiText::UiText(glm::vec2 position, std::string text, float textScale, glm::vec3 textColor, bool center, TextRenderer * textRenderer)
: UiText(position, text, glm::vec2(textScale, textScale), textColor, center, textRenderer) { }

UiText::UiText(glm::vec2 position, std::string text, glm::vec2 textScale, glm::vec3 textColor, bool center,
	TextRenderer* textRenderer) : UiElement(position, textRenderer->getTextSize(text, textScale), center) {
	this->color = textColor;
	this->textRenderer = textRenderer;
	this->rescaleMode = None;
	scale = textScale;
	setText(text);
	UiText::setPosition(position, center);
}

void UiText::render() {
	textRenderer->renderText(text, actualPosition, scale, center, color);
}

void UiText::setText(const char * text) {
	setText(std::string(text));
}

void UiText::setText(std::string text) {
	this->text = text;
	if(rescaleMode != None) {
		scale = textRenderer->getScaleToFitSize(text, size, rescaleMode);
	}
}

void UiText::setup() {}

void UiText::setPosition(glm::vec2 position, bool center) {
	this->center = center;
	actualPosition = position;
}

ShaderType UiText::getShaderType() {
	return STNone;
}
