#ifndef UICHECKBOX_H
#define UICHECKBOX_H

#include <functional>
#include "UiButton.h"

class UiCheckbox : public UiTexturedElement {
public:
	UiCheckbox(const char* textureIdle, const char* textureHover, const char* textureClicked,
		const char* textureTickIdle, const char* textureTickHover, const char* textureTickClicked,
		glm::vec2 position, glm::vec2 size, bool checked, bool center = true);
	UiCheckbox(glm::vec2 position, bool checked, bool center = true);
	UiCheckbox(glm::vec2 position, glm::vec2 size, bool checked, bool center = true);
	void render(Shader *shader) override;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) override;
	void setCheckboxCallback(std::function<void(bool)> callback);
	UiButtonState getState();
	void setup() override;
	ShaderType getShaderType() override;
protected:
	bool checked;
	Texture textureHover, textureClicked, textureTick, textureTickHover, textureTickClicked;
	UiButtonState state = Idle;
	bool clicked = false, hover = false;
	std::function<void(bool)> callback = [](bool) {};
};

#endif
