#include "UiSliderInt.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"

UiSliderInt::UiSliderInt(const char* textureIdle, const char* textureHover, const char* textureClicked,
	glm::vec2 position, glm::vec2 size, double lineThickness, glm::vec2 buttonSize, int value,
	int min, int max, glm::vec4 lineColor, UiAnchor anchor) : UiTexturedElement(nullptr, position, size, anchor) {
	this->min = min;
	this->max = max;
	this->value = value;
	this->actualValue = value;
	this->buttonSize = buttonSize;
	this->lineThickness = lineThickness;
	this->lineColor = lineColor;
	button = new UiButton(textureIdle, textureHover, textureClicked, glm::vec2(0.0f, 0.0f), buttonSize, Center);
	button->localTransform.setPosition(Global::remap(static_cast<double>(value), static_cast<double>(min), static_cast<double>(max), 0.0, size.x) + actualPosition.x, actualPosition.y + size.y / 2.0f, 0.0f);
	addChild(button);
	UiSliderInt::setPosition(position, anchor);
	//setup();
}

UiSliderInt::UiSliderInt(glm::vec2 position, glm::vec2 size, double lineThickness, int value, int min, int max,
	glm::vec4 lineColor, UiAnchor anchor) :
	UiSliderInt(BTN_SHORT_IDLE, BTN_SHORT_HOVER, BTN_SHORT_CLICKED, position, size, lineThickness, glm::vec2(size.y, size.y), value, min, max, lineColor, anchor) {}

void UiSliderInt::render(Shader *shader) {
	UiTexturedElement::render(shader);
	shader->setColor(lineColor);
	glBindVertexArray(vao);
	glBindVertexBuffer(0, vbo, 0, sizeof(UiVertex));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
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
		button->localTransform.setPosition(Global::remap(static_cast<double>(value), static_cast<double>(min), static_cast<double>(max), 0.0, size.x) + actualPosition.x, actualPosition.y + size.y / 2.0f, 0.0f);
	} else {
		moving = false;
	}
}

void UiSliderInt::setup() {
	this->button->setup();

	const float minX = actualPosition.x;
	const float minY = actualPosition.y + (size.y - lineThickness) / 2.0f;

	UiVertex vertices[4];

	vertices[0].Position = glm::vec2(minX, minY + lineThickness);
	vertices[1].Position = glm::vec2(minX, minY);
	vertices[2].Position = glm::vec2(minX + size.x, minY);
	vertices[3].Position = glm::vec2(minX + size.x, minY + lineThickness);

	std::vector<UiVertex> data;
	data.push_back(vertices[0]);
	data.push_back(vertices[2]);
	data.push_back(vertices[1]);
	data.push_back(vertices[0]);
	data.push_back(vertices[3]);
	data.push_back(vertices[2]);

	if (vbo != 0) {
		glDeleteBuffers(1, &vbo);
	}
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(UiVertex), &data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UiVertex), static_cast<void*>(nullptr));

	glBindVertexArray(0);
	data.clear();
}

ShaderType UiSliderInt::getShaderType() {
	return STUiColor;
}

UiSliderInt::~UiSliderInt() {
	delete button;
}
