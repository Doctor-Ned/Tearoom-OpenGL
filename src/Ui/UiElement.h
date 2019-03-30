#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "Headers.h"
#include "Render/Shader.h"

struct UiVertex {
	glm::vec2 Position;
};

struct UiTextureVertex {
	glm::vec2 Position;
	glm::vec2 TexCoords;
};

class UiElement {
public:
	UiElement(Shader* shader, const char* texture, glm::vec2 position, glm::vec2 size, bool center = true);
	virtual void render();
	virtual void mouse_callback(GLFWwindow* window, double xpos, double ypos) {}
	virtual void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {}
	virtual void setPosition(glm::vec2 position, bool center = true);
	glm::vec2 getPosition();
	glm::vec2 getCenter();
	glm::vec2 getSize();
	void reworkProjection();
	virtual ~UiElement();
protected:
	glm::mat4 projection;
	virtual void setup() = 0;
	Texture texture;
	Shader* shader;
	glm::vec2 actualPosition{};
	glm::vec2 size{};
	bool center;
	GLuint vao, vbo = 0;
};

#endif
