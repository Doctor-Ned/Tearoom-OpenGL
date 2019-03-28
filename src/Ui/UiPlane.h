#ifndef UIPLANE_H
#define UIPLANE_H
#include "UiElement.h"

class UiPlane : public UiElement {
public:
	UiPlane(Shader* shader, const char* texture, glm::vec2 position, glm::vec2 size, bool center = true);
	void render() override;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) override;
};

#endif
