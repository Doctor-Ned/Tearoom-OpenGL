#ifndef UIBUTTON_H
#define UIBUTTON_H

#include <functional>
#include "UiTexturedElement.h"

enum UiButtonState {
	Idle,
	Hover,
	Clicked
};


class UiButton : public UiTexturedElement {
public:
	UiButton(const char* textureIdle, const char* textureHover, const char* textureClicked,
	         glm::vec2 position, glm::vec2 size, bool center = true);
	UiButton(glm::vec2 position, bool center = true);
	void render() override;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) override;
	void setButtonCallback(std::function<void()> callback);
	UiButtonState getState();
	void setup() override;
	ShaderType getShaderType() override;
protected:
	Texture textureHover, textureClicked;
	UiButtonState state = Idle;
	bool clicked = false, hover = false;
	std::function<void()> callback = []() {};
};

#endif
