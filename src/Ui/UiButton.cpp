#include "UiButton.h"
#include "GLFW/glfw3.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"

UiButton::UiButton(const char* textureIdle, const char* textureHover, const char* textureClicked,
				   glm::vec2 position, glm::vec2 size, UiAnchor anchor) : UiPlane(textureIdle, position, size, anchor) {
	this->textureHover = AssetManager::getInstance()->getTexture(textureHover);
	this->textureClicked = AssetManager::getInstance()->getTexture(textureClicked);
	setup();
}

UiButton::UiButton(glm::vec2 position, UiAnchor anchor) : UiButton(position, createScaledSize(BASE_LONG_BTN_WIDTH, BASE_LONG_BTN_HEIGHT), anchor) {}

UiButton::UiButton(glm::vec2 position, glm::vec2 size, UiAnchor anchor) : UiButton(BTN_LONG_IDLE, BTN_LONG_HOVER, BTN_LONG_CLICKED, position, size, anchor) {}

void UiButton::render(Shader *shader) {
	UiTexturedElement::render(shader);
	Texture* txt;
	switch (state) {
		default:
			txt = &texture;
			break;
		case Hover:
			txt = &textureHover;
			break;
		case Clicked:
			txt = &textureClicked;
			break;
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, txt->id);
	glBindVertexArray(vao);
	glBindVertexBuffer(0, vbo, 0, sizeof(UiTextureVertex));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void UiButton::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (!active)return;
	UiElement::mouse_callback(window, xpos, ypos);
	glm::vec2 resPosition = getRescaledModeledPosition(), resSize = getRescaledModeledSize();
	double minX = resPosition.x, maxX = resPosition.x + resSize.x, minY = resPosition.y, maxY = resPosition.y + resSize.y;
	if (hover != (xpos > minX && xpos < maxX && ypos > minY && ypos < maxY)) {
		hover = !hover;
		if (hover) {
			state = clicked ? Clicked : Hover;
			if (!clicked) {
				runCallbacks(Hover);
			}
		} else if (!clicked) {
			state = Idle;
			runCallbacks(Idle);
		}
	}
}

void UiButton::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (!active)return;
	UiElement::mouse_button_callback(window, button, action, mods);
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS && hover) {
			clicked = true;
			state = Clicked;
		} else if (action == GLFW_RELEASE) {
			if (clicked) {
				clicked = false;
				if (hover) {
					state = Hover;
					runCallbacks(Clicked);
				} else {
					state = Idle;
					runCallbacks(Idle);
				}
			}
		}
	}
}

void UiButton::addClickCallback(std::function<void()> callback) {
	addCallback(Clicked, callback);
}

void UiButton::addHoverCallback(std::function<void()> callback) {
	addCallback(Hover, callback);
}

void UiButton::addLeaveCallback(std::function<void()> callback) {
	addCallback(Idle, callback);
}

void UiButton::clearClickCallbacks() {
	clearCallbacks(Clicked);
}

void UiButton::clearHoverCallbacks() {
	clearCallbacks(Hover);
}

void UiButton::clearLeaveCallbacks() {
	clearCallbacks(Idle);
}

void UiButton::clearCallbacks() {
	clearCallbacks(Clicked);
	clearCallbacks(Hover);
	clearCallbacks(Idle);
}

UiButtonState UiButton::getState() {
	return state;
}

void UiButton::runCallbacks(UiButtonState state) {
	for (auto &pair : callbacks) {
		if (pair.first == state) {
			for (auto &callback : pair.second) {
				callback();
			}
			break;
		}
	}
}

void UiButton::addCallback(UiButtonState state, std::function<void()> callback) {
	for (auto &pair : callbacks) {
		if (pair.first == state) {
			pair.second.push_back(callback);
			return;
		}
	}
	callbacks.emplace(state, std::vector <std::function<void()>> {callback});
}

void UiButton::clearCallbacks(UiButtonState state) {
	for (auto &pair : callbacks) {
		if (pair.first == state) {
			pair.second.clear();
			break;
		}
	}
}
