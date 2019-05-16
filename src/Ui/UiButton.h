#ifndef UIBUTTON_H
#define UIBUTTON_H

#include <functional>
#include "UiPlane.h"
#include "UiCanvas.h"
#include "UiText.h"

enum UiButtonState {
	Idle,
	Hover,
	Clicked
};


class UiButton : public UiPlane {
public:
	UiButton(const char* textureIdle, const char* textureHover, const char* textureClicked,
	         glm::vec2 position, glm::vec2 size, UiAnchor anchor = Center);
	UiButton(glm::vec2 position, UiAnchor anchor = Center);
	UiButton(glm::vec2 position, glm::vec2 size, UiAnchor anchor = Center);
	void render(Shader *shader) override;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) override;
	void setButtonCallback(std::function<void()> callback);
	void setHooverCallback(std::function<void()> hooverCallback);
	void setHooverDefaultState(std::function<void()> hooverDefaultState);
	using UiElement::setPosition;
	UiButtonState getState();
protected:
	Texture textureHover, textureClicked;
	UiButtonState state = Idle;
	bool clicked = false, hover = false;
	std::function<void()> callback = []() {};
	std::function<void()> hooverCallback = []() {};
	std::function<void()> hooverDefaultState = []() {};
};

#endif
