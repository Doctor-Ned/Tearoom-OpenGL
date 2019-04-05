#include "UiButton.h"
#include "GLFW/glfw3.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"

UiButton::UiButton(const char* textureIdle, const char* textureHover, const char* textureClicked,
	glm::vec2 position, glm::vec2 size, bool center) : UiTexturedElement(textureIdle, position, size, center) {
	this->textureHover = AssetManager::getInstance()->getTexture(textureHover);
	this->textureClicked = AssetManager::getInstance()->getTexture(textureClicked);
	this->shader = AssetManager::getInstance()->getShader(getShaderType());
	setup();
}

UiButton::UiButton(glm::vec2 position, bool center) : UiButton(position, createScaledSize(BASE_LONG_BTN_WIDTH, BASE_LONG_BTN_HEIGHT), center) {}

UiButton::UiButton(glm::vec2 position, glm::vec2 size, bool center) : UiButton(BTN_LONG_IDLE, BTN_LONG_HOVER, BTN_LONG_CLICKED, position,size, center) {}

void UiButton::render() {
	UiElement::render();
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
	glUseProgram(0);
}

void UiButton::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	glm::vec2 resPosition = getRescaledPosition(), resSize = getRescaledSize();
	double minX = resPosition.x, maxX = resPosition.x + resSize.x, minY = resPosition.y, maxY = resPosition.y + size.y;
	if (hover != (xpos > minX && xpos < maxX && ypos > minY && ypos < maxY)) {
		hover = !hover;
		if (hover) {
			state = clicked ? Clicked : Hover;
		} else if (!clicked) {
			state = Idle;
		}
	}
}

void UiButton::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS && hover) {
			clicked = true;
			state = Clicked;
		} else if (action == GLFW_RELEASE) {
			if (clicked) {
				clicked = false;
				if (hover) {
					state = Hover;
					if (callback != nullptr) {
						callback();
					}
				} else {
					state = Idle;
				}
			}
		}
	}
}

void UiButton::setup() {
	const float minX = actualPosition.x;
	const float minY = actualPosition.y;

	UiTextureVertex vertices[4];

	vertices[0].TexCoords = glm::vec2(0.0f, 1.0f);
	vertices[1].TexCoords = glm::vec2(0.0f, 0.0f);
	vertices[2].TexCoords = glm::vec2(1.0f, 0.0f);
	vertices[3].TexCoords = glm::vec2(1.0f, 1.0f);

	vertices[0].Position = glm::vec2(minX, minY + size.y);
	vertices[1].Position = glm::vec2(minX, minY);
	vertices[2].Position = glm::vec2(minX + size.x, minY);
	vertices[3].Position = glm::vec2(minX + size.x, minY + size.y);

	std::vector<UiTextureVertex> data;
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
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(UiTextureVertex), &data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UiTextureVertex), (void*)nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UiTextureVertex),
		(void*)offsetof(UiTextureVertex, TexCoords));

	glBindVertexArray(0);
	data.clear();
}

ShaderType UiButton::getShaderType() {
	return STUiTexture;
}

void UiButton::setButtonCallback(std::function<void()> callback) {
	this->callback = callback;
}

void UiButton::setPosition(glm::vec2 position, bool center) {
	UiTexturedElement::setPosition(position, center);
	setup();
}

UiButtonState UiButton::getState() {
	return state;
}
