#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "UiElement.h"
#include <functional>

enum UiButtonState {
	Idle,
	Hover,
	Clicked
};


class UiButton : public UiElement {
public:
	UiButton(Shader* shader, const char* textureIdle, const char* textureHover, const char* textureClicked,
	         glm::vec2 position, glm::vec2 size, bool center = true);
	void render() override;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) override;
	void setButtonCallback(std::function<void()> callback);
	void setPosition(glm::vec2 position, bool center = true);
	UiButtonState getState();
protected:
	void setup();
	Texture textureHover, textureClicked;
	UiButtonState state = Idle;
	bool clicked = false, hover = false;
	std::function<void()> callback = []() {};
};

#endif
