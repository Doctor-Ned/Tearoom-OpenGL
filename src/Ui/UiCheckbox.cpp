#include "UiCheckbox.h"
#include "GLFW/glfw3.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"

UiCheckbox::UiCheckbox(const char* textureIdle, const char* textureHover, const char* textureClicked,
	const char* textureTickIdle, const char* textureTickHover, const char* textureTickClicked, glm::vec2 position,
	glm::vec2 size, bool checked, UiAnchor anchor) : UiTexturedElement(textureIdle, position, size, anchor) {
	this->textureHover = AssetManager::getInstance()->getTexture(textureHover);
	this->textureClicked = AssetManager::getInstance()->getTexture(textureClicked);
	this->textureTick = AssetManager::getInstance()->getTexture(textureTickIdle);
	this->textureTickHover = AssetManager::getInstance()->getTexture(textureTickHover);
	this->textureTickClicked = AssetManager::getInstance()->getTexture(textureTickClicked);
	this->checked = checked;
	setup();
}

UiCheckbox::UiCheckbox(glm::vec2 position, bool checked, UiAnchor anchor) : UiCheckbox(position, createSizeScaledByHeight(BASE_BTN_SIZE), checked, anchor) {}

UiCheckbox::UiCheckbox(glm::vec2 position, glm::vec2 size, bool checked, UiAnchor anchor) :
	UiCheckbox(BTN_SHORT_IDLE, BTN_SHORT_HOVER, BTN_SHORT_CLICKED, BTN_TICK_IDLE, BTN_TICK_HOVER, BTN_TICK_CLICKED, position, size, checked, anchor) {}

void UiCheckbox::render(Shader *shader) {
	CHECK_GL_ERROR();
	UiTexturedElement::render(shader);
	CHECK_GL_ERROR();
	Texture* txt;
	switch (state) {
		default:
			txt = checked ? &textureTick : &texture;
			break;
		case Hover:
			txt = checked ? &textureTickHover : &textureHover;
			break;
		case Clicked:
			txt = checked ? &textureTickClicked : &textureClicked;
			break;
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, txt->id);
	glBindVertexArray(vao);
	CHECK_GL_ERROR();
	glBindVertexBuffer(0, vbo, 0, sizeof(UiTextureVertex));
	CHECK_GL_ERROR();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	CHECK_GL_ERROR();
	glBindVertexArray(0);
	CHECK_GL_ERROR();
	
}

void UiCheckbox::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (!active)return;
	UiElement::mouse_callback(window, xpos, ypos);
	glm::vec2 resPosition = getRescaledModeledPosition(), resSize = getRescaledModeledSize();
	double minX = resPosition.x, maxX = resPosition.x + resSize.x, minY = resPosition.y, maxY = resPosition.y + resSize.y;
	if (hover != (xpos > minX && xpos < maxX && ypos > minY && ypos < maxY)) {
		hover = !hover;
		if (hover) {
			state = clicked ? Clicked : Hover;
		} else if (!clicked) {
			state = Idle;
		}
	}
}

void UiCheckbox::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
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
					if (callback != nullptr) {
						checked = !checked;
						callback(checked);
					}
				} else {
					state = Idle;
				}
			}
		}
	}
}

void UiCheckbox::setup() {
	UiTextureVertex vertices[4];

	vertices[0].TexCoords = glm::vec2(0.0f, 1.0f);
	vertices[1].TexCoords = glm::vec2(0.0f, 0.0f);
	vertices[2].TexCoords = glm::vec2(1.0f, 0.0f);
	vertices[3].TexCoords = glm::vec2(1.0f, 1.0f);

	vertices[0].Position = glm::vec2(0.0f,  size.y);
	vertices[1].Position = glm::vec2(0.0f, 0.0f);
	vertices[2].Position = glm::vec2(size.x, 0.0f);
	vertices[3].Position = glm::vec2(size.x, size.y);

	std::vector<UiTextureVertex> data;
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
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(UiTextureVertex), &data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UiTextureVertex), static_cast<void*>(nullptr));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UiTextureVertex), reinterpret_cast<void*>(offsetof(UiTextureVertex, TexCoords)));

	glBindVertexArray(0);
	data.clear();
}

ShaderType UiCheckbox::getShaderType() {
	return STUiTexture;
}

void UiCheckbox::setCheckboxCallback(std::function<void(bool)> callback) {
	this->callback = callback;
}

UiButtonState UiCheckbox::getState() {
	return state;
}
