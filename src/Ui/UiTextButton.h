#ifndef UITEXTBUTTON_H
#define UITEXTBUTTON_H

#include "UiButton.h"
#include "Render/TextRenderer.h"
#include "Scene/GameManager.h"
#include "UiText.h"

static const float BASE_TEXT_HEIGHT_SCALE = 0.75f;

class UiTextButton : public UiButton {
public:
	UiTextButton(const char* textureIdle, const char* textureHover, const char* textureClicked,
		glm::vec2 position, glm::vec2 size, const char* text, float textHeightScale = BASE_TEXT_HEIGHT_SCALE, glm::vec3 textColor = glm::vec3(0.3f, 0.3f, 0.3f), UiAnchor anchor = Center,
		TextRenderer* textRenderer = AssetManager::getInstance()->getTextRenderer());
	UiTextButton(glm::vec2 position, const char* text, glm::vec3 textColor = glm::vec3(0.3f, 0.3f, 0.3f), UiAnchor anchor = Center,
		TextRenderer* textRenderer = AssetManager::getInstance()->getTextRenderer());
	UiTextButton(glm::vec2 position, glm::vec2 size, const char* text, glm::vec3 textColor = glm::vec3(0.3f, 0.3f, 0.3f), UiAnchor anchor = Center,
		TextRenderer* textRenderer = AssetManager::getInstance()->getTextRenderer());
	void setText(char* text);
	using UiElement::setPosition;
	void setPosition(glm::vec2 position, UiAnchor anchor) override;
	void setup() override;
protected:
	UiText *text;
	double moveX, moveY;
	bool moving = false;
};

#endif
