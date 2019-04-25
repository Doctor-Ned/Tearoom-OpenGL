#include "UiSlider.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"

UiSlider::UiSlider(const char* textureIdle, const char* textureHover, const char* textureClicked,
	glm::vec2 position, glm::vec2 size, double lineThickness, glm::vec2 buttonSize, float value,
	float min, float max, glm::vec4 lineColor, UiAnchor anchor) : UiTexturedElement(nullptr, position, size, anchor) {
	this->min = min;
	this->max = max;
	this->value = value;
	this->buttonSize = buttonSize;
	this->lineThickness = lineThickness;
	this->lineColor = lineColor;
	button = new UiButton(textureIdle, textureHover, textureClicked, position, buttonSize, Center);
	UiSlider::setPosition(position, anchor);
	//setup();
}

UiSlider::UiSlider(glm::vec2 position, glm::vec2 size, double lineThickness, float value, float min, float max,
	glm::vec4 lineColor, UiAnchor anchor) :
	UiSlider(BTN_SHORT_IDLE, BTN_SHORT_HOVER, BTN_SHORT_CLICKED, position, size, lineThickness, glm::vec2(size.y, size.y), value, min, max, lineColor, anchor) {}

void UiSlider::render(Shader *shader) {
	UiTexturedElement::render(shader);
	shader->setColor(lineColor);
	glBindVertexArray(vao);
	glBindVertexBuffer(0, vbo, 0, sizeof(UiVertex));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	button->setPosition(glm::vec2(Global::remap(value, min, max, 0.0, size.x) + actualPosition.x,
		actualPosition.y + size.y / 2.0f));
	Shader *shad = AssetManager::getInstance()->getShader(button->getShaderType());
	shad->use();
	button->render(shad);
	shader->use();
}

void UiSlider::setCallback(std::function<void(float)> callback) {
	this->callback = callback;
}

void UiSlider::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	button->mouse_callback(window, xpos, ypos);
	if (button->getState() == Clicked) {
		if (moving) {
			glm::vec2 resPosition = getRescaledPosition(), resSize = getRescaledSize();
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
				const double currentX = Global::remap(value, min, max, 0.0, size.x);
				value = Global::remap(currentX + xpos - moveX, 0.0, size.x, min, max);
				moveX = xpos;
				callback(value);
			}
		} else {
			moving = true;
			moveX = xpos;
		}
	} else {
		moving = false;
	}
}

void UiSlider::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	this->button->mouse_button_callback(window, button, action, mods);
}

void UiSlider::setPosition(glm::vec2 position, UiAnchor anchor) {
	UiElement::setPosition(position, anchor);
	this->button->setPosition(position);
	setup();
}

void UiSlider::setup() {
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

ShaderType UiSlider::getShaderType() {
	return STUiColor;
}

UiSlider::~UiSlider() {
	delete button;
}
