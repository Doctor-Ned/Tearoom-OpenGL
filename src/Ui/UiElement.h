#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "Global.h"
#include "Render/Shader.h"

enum ShaderType;

enum UiRescaleMode {
	Fit,
	MatchHeight,
	MatchWidth,
	Stretch,
	None
};

enum UiAnchor {  //not used yet but i hope this thing will change the world some day
	Left,
	TopLeft,
	Top,
	TopRight,
	Right,
	BottomRight,
	Bottom,
	BottomLeft,
	Center
};

struct UiVertex {
	glm::vec2 Position;
};

struct UiTextureVertex {
	glm::vec2 Position;
	glm::vec2 TexCoords;
};

class UiElement {
public:
	UiElement(glm::vec2 position, glm::vec2 size, bool center = true);
	virtual void render();
	virtual void mouse_callback(GLFWwindow* window, double xpos, double ypos) {}
	virtual void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {}
	virtual void setPosition(glm::vec2 position, bool center = true);
	float getOpacity() const;
	void setOpacity(float opacity);
	glm::vec2 getPosition();
	glm::vec2 getCenter();
	glm::vec2 getSize();
	void updateProjection(float windowWidth, float windowHeight, float screenWidth, float screenHeight);
	virtual ~UiElement();
	virtual ShaderType getShaderType() = 0;
protected:
	static glm::vec2 createScaledSize(float width, float height);
	glm::vec2 createSizeScaledByWidth(float size);
	glm::vec2 createSizeScaledByHeight(float size);
	glm::vec2 getRescaledPosition();
	glm::vec2 getRescaledSize();
	float windowWidth, windowHeight, screenWidth, screenHeight;
	float opacity = 1.0f;
	glm::mat4 projection;
	virtual void setup() = 0;
	Shader* shader;
	glm::vec2 actualPosition{};
	glm::vec2 size{};
	bool center;
	GLuint vao, vbo = 0;
};

#endif
