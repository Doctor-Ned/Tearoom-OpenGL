#ifndef UISLIDER_H
#define UISLIDER_H

#include "Global.h"
#include <functional>
#include "UiButton.h"

class UiSlider : public UiTexturedElement {
public:
	UiSlider(const char* textureIdle, const char* textureHover, const char* textureClicked,
	         glm::vec2 position, glm::vec2 size, double lineThickness, glm::vec2 buttonSize, float value, float min, float max,
	         glm::vec4 lineColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), UiAnchor anchor = Center);
	UiSlider(glm::vec2 position, glm::vec2 size, double lineThickness, float value, float min, float max, glm::vec4 lineColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), UiAnchor anchor = Center);
	void render(Shader *shader) override;
	void setCallback(std::function<void(float)> callback);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	using UiElement::setPosition;
	void setPosition(glm::vec2 position, UiAnchor anchor) override;
	void setup() override;
	ShaderType getShaderType() override;
	~UiSlider();
protected:
	float value, min, max;
	double lineThickness;
	glm::vec2 buttonSize;
	glm::vec4 lineColor;
	UiButton* button;
	double moveX;
	bool moving = false;
	std::function<void(float)> callback = [](float) {};
};

#endif
