#include "UiText.h"

UiText::UiText(glm::vec2 position, glm::vec2 size, const char* text, glm::vec3 textColor, UiRescaleMode rescaleMode,
	UiAnchor anchor, TextRenderer* textRenderer) : UiElement(position, size, anchor) {
	this->color = textColor;
	this->textRenderer = textRenderer;
	this->rescaleMode = rescaleMode;
	this->text = text;
	scale = textRenderer->getScaleToFitSize(text, size, rescaleMode);
	UiText::setPosition(position, anchor);
}

UiText::UiText(glm::vec2 position, glm::vec2 size, std::string text, glm::vec3 textColor, UiRescaleMode rescaleMode,
	UiAnchor anchor, TextRenderer* textRenderer) : UiElement(position, size, anchor) {
	this->color = textColor;
	this->textRenderer = textRenderer;
	this->rescaleMode = rescaleMode;
	this->text = text;
	scale = textRenderer->getScaleToFitSize(text, size, rescaleMode);
	UiText::setPosition(position, anchor);
}

UiText::UiText(glm::vec2 position, std::string text, float textScale, glm::vec3 textColor, UiAnchor anchor, TextRenderer * textRenderer)
: UiText(position, text, glm::vec2(textScale, textScale), textColor, anchor, textRenderer) { }

UiText::UiText(glm::vec2 position, std::string text, glm::vec2 textScale, glm::vec3 textColor, UiAnchor anchor,
	TextRenderer* textRenderer) : UiElement(position, textRenderer->getTextSize(text, textScale), anchor) {
	this->color = textColor;
	this->textRenderer = textRenderer;
	this->rescaleMode = None;
	scale = textScale;
	setText(text);
	UiText::setPosition(position, anchor);
}

void UiText::render(Shader *shader) {
	shader->setModel(worldTransform.getMatrix());
	textRenderer->renderText(shader, text, glm::vec2(0.0f, 0.0f), scale, anchor, color);
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

void UiText::setPosition(glm::vec2 position, UiAnchor anchor) {
	UiElement::setPosition(position, TopLeft);
	this->anchor = anchor;
}

ShaderType UiText::getShaderType() {
	return STText;
}
