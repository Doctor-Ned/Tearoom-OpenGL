#ifndef UISLIDER_H
#define UISLIDER_H

#include "Headers.h"
#include "UiElement.h"
#include <functional>
#include "UiButton.h"

class UiSlider : public UiElement {
public:
	UiSlider(Shader* shader, const char* textureIdle, const char* textureHover, const char* textureClicked,
	         glm::vec2 position, glm::vec2 size, Shader* lineShader
	         , double lineThickness, glm::vec2 buttonSize, float value, float min, float max,
	         glm::vec4 lineColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), bool center = true);
	void render() override;
	void setCallback(std::function<void(float)> callback);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) override;
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
