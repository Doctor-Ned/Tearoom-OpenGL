#pragma once

#include "Global.h"
#include <functional>
#include "UiButton.h"

class UiSliderInt : public UiTexturedElement {
public:
	UiSliderInt(const char* textureIdle, const char* textureHover, const char* textureClicked,
		glm::vec2 position, glm::vec2 size, double lineThickness, glm::vec2 buttonSize, int value, int min, int max,
		glm::vec4 lineColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), UiAnchor anchor = Center);
	UiSliderInt(glm::vec2 position, glm::vec2 size, double lineThickness, int value, int min, int max, glm::vec4 lineColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), UiAnchor anchor = Center);
	void render(Shader *shader) override;
	void setCallback(std::function<void(int)> callback);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void setup() override;
	ShaderType getShaderType() override;
	~UiSliderInt();
protected:
	int value, min, max;
	float actualValue;
	double lineThickness;
	glm::vec2 buttonSize;
	glm::vec4 lineColor;
	UiButton* button;
	double moveX;
	bool moving = false;
	std::function<void(int)> callback = [](int) {};
};