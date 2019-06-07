#include "UiSlider.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"

UiSlider::UiSlider(const char* textureIdle, const char* textureHover, const char* textureClicked,
	glm::vec2 position, glm::vec2 size, double lineThickness, glm::vec2 buttonSize, float value,
	float min, float max, glm::vec4 lineColor, UiAnchor anchor) : UiElement(position, size, anchor) {
	this->min = min;
	this->max = max;
	this->value = value;
	this->buttonSize = buttonSize;
	this->lineThickness = lineThickness;
	this->lineColor = lineColor;
	plane = new UiColorPlane(lineColor, glm::vec2(0.0f, size.y / 2.0f), glm::vec2(size.x, lineThickness), Left);
	addChild(plane);
	button = new UiButton(textureIdle, textureHover, textureClicked, glm::vec2(0.0f, 0.0f), buttonSize, Center);
	button->setPosition(glm::vec2(Global::remap(value, min, max, 0.0, size.x), size.y / 2.0f));
	addChild(button);
	UiSlider::setPosition(position, anchor);
	setup();
}

UiSlider::UiSlider(glm::vec2 position, glm::vec2 size, double lineThickness, float value, float min, float max,
	glm::vec4 lineColor, UiAnchor anchor) :
	UiSlider(BTN_SHORT_IDLE, BTN_SHORT_HOVER, BTN_SHORT_CLICKED, position, size, lineThickness, glm::vec2(size.y, size.y), value, min, max, lineColor, anchor) {}

void UiSlider::render(Shader *shader) {
	UiElement::render(shader);
}

void UiSlider::setCallback(std::function<void(float)> callback) {
	this->callback = callback;
}

void UiSlider::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (!active)return;
	UiElement::mouse_callback(window, xpos, ypos);
	if (button->getState() == Clicked) {
		if (moving) {
			glm::vec2 resPosition = getRescaledModeledPosition(), resSize = getRescaledModeledSize();
			if (xpos < resPosition.x) {
				if (value != min) {
					value = min;
					callback(value);
				}
			} else if (xpos > resPosition.x + resSize.x) {
				if (value != max) {
					value = max;
					callback(value);
				}
			} else {
				const double currentX = Global::remap(value, min, max, 0.0, resSize.x);
				value = Global::remap(currentX + xpos - moveX, 0.0, resSize.x, min, max);
				moveX = xpos;
				callback(value);
			}
		} else {
			moving = true;
			moveX = xpos;
		}
		button->setPosition(glm::vec2(Global::remap(value, min, max, 0.0, size.x), size.y / 2.0f));
	} else {
		moving = false;
	}
}

void UiSlider::setup() {
	this->plane->setup();
	this->button->setup();
}

ShaderType UiSlider::getShaderType() {
	return STNone;
}

UiSlider::~UiSlider() {
	delete button;
}
