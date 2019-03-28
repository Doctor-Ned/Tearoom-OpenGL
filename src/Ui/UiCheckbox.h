#ifndef UICHECKBOX_H
#define UICHECKBOX_H

#include <functional>
#include "UiButton.h"

class UiCheckbox : public UiElement {
public:
	UiCheckbox(Shader* shader, const char* textureIdle, const char* textureHover, const char* textureClicked,
		const char* textureTickIdle, const char* textureTickHover, const char* textureTickClicked,
		glm::vec2 position, glm::vec2 size, bool checked, bool center = true);
	void render() override;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) override;
	void setCheckboxCallback(std::function<void(bool)> callback);
	void setPosition(glm::vec2 position, bool center = true);
	UiButtonState getState();
protected:
	void setup();
	bool checked;
	Texture textureHover, textureClicked, textureTick, textureTickHover, textureTickClicked;
	UiButtonState state = Idle;
	bool clicked = false, hover = false;
	std::function<void(bool)> callback = [](bool) {};
};

#endif
