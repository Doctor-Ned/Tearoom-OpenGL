#pragma once

#include "Global.h"
#include "Render/Shader.h"
#include "Scene/Transform.h"
#include "Scene/ComposedTransform.h"

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
	UiElement(glm::vec2 position, glm::vec2 size, UiAnchor anchor = Center);
	virtual void render(Shader *shader);
	virtual void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	virtual void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	virtual void setPosition(glm::vec2 position, UiAnchor anchor);
	void setPosition(glm::vec2 position);
	float getOpacity() const;
	void setOpacity(float opacity);
	glm::vec2 getPosition() const;
	glm::vec2 getCenter() const;
	glm::vec2 getSize() const;
	void removeChild(UiElement* element);
	void setParent(UiElement *element);
	void addChild(UiElement *element);
	UiElement *getChild(int index);
	std::vector<UiElement*> getChildren();
	void updateDrawData();
	static void updateProjection(float windowWidth, float windowHeight, float screenWidth, float screenHeight);
	virtual ~UiElement();
	virtual ShaderType getShaderType() = 0;
	static glm::mat4 getProjection();
	Transform worldTransform;
	ComposedTransform localTransform;
	void setActive(bool active);
	bool isActive();
protected:
	UiElement *parent = nullptr;
	std::vector<UiElement*> children;
	virtual void updateWorld();
	bool active = true;
	static float windowWidth;
	static float windowHeight;
	static float screenWidth;
	static float screenHeight;
	static glm::mat4 projection;
	static glm::vec2 createScaledSize(float width, float height);
	glm::vec2 createSizeScaledByWidth(float size);
	glm::vec2 createSizeScaledByHeight(float size);
	glm::vec2 getRescaledPosition();
	glm::vec2 getRescaledSize();
	glm::vec2 getRescaledModeledPosition();
	glm::vec2 getRescaledModeledSize();
	float opacity = 1.0f;
	virtual void setup();
	bool dirty = true;
	glm::vec2 actualPosition{};
	glm::vec2 modeledPosition{};
	glm::vec2 size{};
	glm::vec2 modeledSize{};
	GLuint vao, vbo = 0;
	UiAnchor anchor;
};