#ifndef SCENE_H
#define SCENE_H

#include "Headers.h"

class Scene {
public:
	virtual void render() = 0;
	virtual void update(double deltaTime) = 0;
	virtual void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
		this->windowWidth = windowWidth;
		this->windowHeight = windowHeight;
		this->screenWidth = screenWidth;
		this->screenHeight = screenHeight;
		windowCenterX = windowWidth / 2.0f;
		windowCenterY = windowHeight / 2.0f;
	}
	virtual void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {};
	virtual void mouse_callback(GLFWwindow* window, double xpos, double ypos) {};
	virtual void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {};
protected:
	float windowWidth, windowHeight, windowCenterX, windowCenterY, screenWidth, screenHeight;
};

#endif
