#ifndef UICOLORPLANE_H
#define UICOLORPLANE_H
#include "UiElement.h"

class UiColorPlane : public UiElement {
public:
	UiColorPlane(Shader* shader, glm::vec4 color, glm::vec2 position, glm::vec2 size, bool center = true);
	void render() override;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) override;
protected:
	glm::vec4 color;
};

#endif
