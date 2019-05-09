#include "UiSliderInt.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"

UiSliderInt::UiSliderInt(const char* textureIdle, const char* textureHover, const char* textureClicked,
	glm::vec2 position, glm::vec2 size, double lineThickness, glm::vec2 buttonSize, int value,
	int min, int max, glm::vec4 lineColor, UiAnchor anchor) : UiElement(position, size, anchor) {
	this->min = min;
	this->max = max;
	this->value = value;
	this->actualValue = value;
	this->buttonSize = buttonSize;
	this->lineThickness = lineThickness;
	this->lineColor = lineColor;
	plane = new UiColorPlane(lineColor, glm::vec2(0.0f, size.y/2.0f), glm::vec2(size.x, lineThickness), Left);
	addChild(plane);
	button = new UiButton(textureIdle, textureHover, textureClicked, glm::vec2(0.0f, 0.0f), buttonSize, Center);
	button->setPosition(glm::vec2(Global::remap(static_cast<double>(value), static_cast<double>(min), static_cast<double>(max), 0.0, size.x), size.y / 2.0f));
	addChild(button);
	UiSliderInt::setPosition(position, anchor);
	setup();
}

UiSliderInt::UiSliderInt(glm::vec2 position, glm::vec2 size, double lineThickness, int value, int min, int max,
	glm::vec4 lineColor, UiAnchor anchor) :
	UiSliderInt(BTN_SHORT_IDLE, BTN_SHORT_HOVER, BTN_SHORT_CLICKED, position, size, lineThickness, glm::vec2(size.y, size.y), value, min, max, lineColor, anchor) {}

void UiSliderInt::render(Shader *shader) {
	UiElement::render(shader);
}

void UiSliderInt::setCallback(std::function<void(int)> callback) {
	this->callback = callback;
}

void UiSliderInt::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	UiElement::mouse_callback(window, xpos, ypos);
	if (button->getState() == Clicked) {
		if (moving) {
			glm::vec2 resPosition = getRescaledModeledPosition(), resSize = getRescaledModeledSize();
			if (xpos < resPosition.x) {
				if (value != min || actualValue != min) {
					actualValue = min;
					if (value != min) {
						callback(min);
					}
					value = min;
				}
			} else if (xpos > resPosition.x + resSize.x) {
				if (value != max || actualValue != max) {
					actualValue = max;
					if (value != max) {
						callback(max);
					}
					value = max;
				}
			} else {
				const double currentX = Global::remap(actualValue, static_cast<float>(min), static_cast<float>(max), 0.0, resSize.x);
				actualValue = Global::remap(currentX + xpos - moveX, 0.0, resSize.x, static_cast<float>(min), static_cast<float>(max));
				moveX = xpos;
				int newValue = round(actualValue);
				if (newValue < min) {
					newValue = min;
				} else if (newValue > max) {
					newValue = max;
				}
				if (newValue != value) {
					callback(newValue);
				}
				//actualValue = value;
				value = newValue;
			}

		} else {
			moving = true;
			moveX = xpos;
		}
		button->setPosition(glm::vec2(Global::remap(static_cast<double>(value), static_cast<double>(min), static_cast<double>(max), 0.0, size.x), size.y / 2.0f));
	} else {
		moving = false;
	}
}

void UiSliderInt::setup() {
	this->plane->setup();
	this->button->setup();
}

ShaderType UiSliderInt::getShaderType() {
	return STNone;
}

UiSliderInt::~UiSliderInt() {
	delete button;
}
