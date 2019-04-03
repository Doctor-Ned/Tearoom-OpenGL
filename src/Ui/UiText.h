#pragma once

#include "UiElement.h"
#include "Render/TextRenderer.h"
#include "Scene/SceneManager.h"

class UiText : public UiElement {
public:
	UiText(glm::vec2 position, glm::vec2 size, const char* text, glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f), UiRescaleMode rescaleMode = Fit, bool center = true,
		TextRenderer* textRenderer = SceneManager::getInstance()->getTextRenderer());
	UiText(glm::vec2 position, glm::vec2 size, std::string text, glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f), UiRescaleMode rescaleMode = Fit, bool center = true,
		TextRenderer* textRenderer = SceneManager::getInstance()->getTextRenderer());
	void render() override;
	void setText(const char* text);
	void setText(std::string text);
	void setup() override;
	void setPosition(glm::vec2 position, bool center) override;
	ShaderType getShaderType() override;
protected:
	bool center;
	UiRescaleMode rescaleMode;
	TextRenderer* textRenderer;
	std::string text;
	glm::vec2 scale;
	glm::vec3 color;
};