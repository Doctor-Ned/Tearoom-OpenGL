#include "UiTextButton.h"

UiTextButton::UiTextButton(const char* textureIdle, const char* textureHover, const char* textureClicked,
	glm::vec2 position, glm::vec2 size, const char* text, float textHeightScale,
	glm::vec3 textColor, bool center, TextRenderer* textRenderer) : UiButton(textureIdle, textureHover, textureClicked, position, size, center) {
	this->text = new UiText(position, size * textHeightScale, text, textColor, MatchHeight, true, textRenderer);
	UiTextButton::setPosition(position, center);
	setup();
}

UiTextButton::UiTextButton(glm::vec2 position, const char* text, glm::vec3 textColor,
	bool center, TextRenderer* textRenderer) : UiTextButton(position, createScaledSize(BASE_LONG_BTN_WIDTH, BASE_LONG_BTN_HEIGHT), text, textColor, center, textRenderer) {}

UiTextButton::UiTextButton(glm::vec2 position, glm::vec2 size, const char* text, glm::vec3 textColor,
	bool center, TextRenderer* textRenderer) : UiTextButton(BTN_LONG_IDLE, BTN_LONG_HOVER, BTN_LONG_CLICKED,
		position, size, text, BASE_TEXT_HEIGHT_SCALE, textColor, center, textRenderer) {}

void UiTextButton::render(Shader *shader) {
	UiButton::render(shader);
	Shader *shad2 = AssetManager::getInstance()->getShader(text->getShaderType());
	shad2->use();
	text->render(shad2);
	shader->use();
}

void UiTextButton::setText(char* text) {
	this->text->setText(text);
}

void UiTextButton::setPosition(glm::vec2 position, bool center) {
	UiElement::setPosition(position, center);
	this->text->setPosition(glm::vec2(actualPosition.x + size.x / 2.0f, actualPosition.y + size.y / 2.0f), true);
}

void UiTextButton::setup() { }
