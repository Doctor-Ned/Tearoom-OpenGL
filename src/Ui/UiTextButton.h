#ifndef UITEXTBUTTON_H
#define UITEXTBUTTON_H

#include "UiButton.h"
#include "Render/TextRenderer.h"

class UiTextButton : public UiButton {
public:
	UiTextButton(const char* textureIdle, const char* textureHover, const char* textureClicked,
	             glm::vec2 position, glm::vec2 size, TextRenderer* textRenderer, const char* text
	             , glm::vec3 textColor = glm::vec3(0.3f, 0.3f, 0.3f), bool center = true);
	UiTextButton(glm::vec2 position, TextRenderer* textRenderer, const char* text, glm::vec3 textColor = glm::vec3(0.3f, 0.3f, 0.3f), bool center = true);
	void render() override;
	void setText(char* text);
	void setPosition(glm::vec2 position, bool center) override;
	void setup() override;
protected:
	TextRenderer* textRenderer;
	const char* text;
	double moveX, moveY;
	bool moving = false;
	glm::vec3 color;
	glm::vec2 textAnchor;
};

#endif
