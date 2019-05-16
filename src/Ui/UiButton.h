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
	void addClickCallback(std::function<void()> callback);
	void addHoverCallback(std::function<void()> callback);
	void addLeaveCallback(std::function<void()> callback);
	void clearClickCallbacks();
	void clearHoverCallbacks();
	void clearLeaveCallbacks();
	void clearCallbacks();
	using UiElement::setPosition;
	UiButtonState getState();
protected:
	void runCallbacks(UiButtonState state);
	void addCallback(UiButtonState state, std::function<void()> callback);
	void clearCallbacks(UiButtonState state);
	Texture textureHover, textureClicked;
	UiButtonState state = Idle;
	bool clicked = false, hover = false;
	std::map<UiButtonState, std::vector<std::function<void()>>> callbacks;
};

#endif
