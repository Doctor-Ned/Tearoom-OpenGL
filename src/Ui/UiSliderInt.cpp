#include "UiSliderInt.h"
#include "Scene/SceneManager.h"

UiSliderInt::UiSliderInt(const char* textureIdle, const char* textureHover, const char* textureClicked,
	glm::vec2 position, glm::vec2 size, double lineThickness, glm::vec2 buttonSize, int value,
	int min, int max, glm::vec4 lineColor, bool center) : UiTexturedElement(nullptr, position, size, center) {
	this->min = min;
	this->max = max;
	this->value = value;
	this->actualValue = value;
	this->buttonSize = buttonSize;
	this->lineThickness = lineThickness;
	this->lineColor = lineColor;
	this->shader = SceneManager::getInstance()->getShader(getShaderType());
	button = new UiButton(textureIdle, textureHover, textureClicked, position, buttonSize, true);
	UiSliderInt::setPosition(position, center);
	//setup();
}

UiSliderInt::UiSliderInt(glm::vec2 position, glm::vec2 size, double lineThickness, int value, int min, int max,
	glm::vec4 lineColor, bool center) :
	UiSliderInt(BTN_SHORT_IDLE, BTN_SHORT_HOVER, BTN_SHORT_CLICKED, position, size, lineThickness, createSizeScaledByHeight(BASE_BTN_SIZE), value, min, max, lineColor, center) {}

void UiSliderInt::render() {
	UiTexturedElement::render();
	shader->setColor(lineColor);
	glBindVertexArray(vao);
	glBindVertexBuffer(0, vbo, 0, sizeof(UiVertex));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
	button->setPosition(glm::vec2(Global::remap(static_cast<double>(value), static_cast<double>(min), static_cast<double>(max), 0.0, size.x) + actualPosition.x,
		actualPosition.y + size.y / 2.0f));
	button->render();
}

void UiSliderInt::setCallback(std::function<void(int)> callback) {
	this->callback = callback;
}

void UiSliderInt::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	button->mouse_callback(window, xpos, ypos);
	if (button->getState() == Clicked) {
		if (moving) {
			glm::vec2 resPosition = getRescaledPosition(), resSize = getRescaledSize();
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
				const double currentX = Global::remap(actualValue, static_cast<float>(min), static_cast<float>(max), 0.0, size.x);
				actualValue = Global::remap(currentX + xpos - moveX, 0.0, size.x, static_cast<float>(min), static_cast<float>(max));
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
	} else {
		moving = false;
	}
}

void UiSliderInt::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	this->button->mouse_button_callback(window, button, action, mods);
}

void UiSliderInt::setPosition(glm::vec2 position, bool center) {
	UiElement::setPosition(position, center);
	this->button->setPosition(position, center);
	setup();
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

	shader->use();

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
	return STColor;
}

UiSliderInt::~UiSliderInt() {
	delete button;
}
