#pragma once

#include "Global.h"
#include "Render/Shader.h"
#include "Scene/Transform.h"
#include "InterfaceTransform.h"

const glm::vec4 MENU_BACKGROUND_COLOR(0.761f, 0.761f, 0.761f, 1.0f);
const glm::vec4 MENU_TEXT_COLOR(0.302f, 0.302f, 0.302f, 1.0f);

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
	glm::vec2 getPosition();
	glm::vec2 getModeledPosition();
	glm::vec2 getModeledCenter();
	glm::vec2 getCenter();
	glm::vec2 getSize() const;
	void removeChild(UiElement* element);
	void setParent(UiElement *element);
	void addChild(UiElement *element);
	UiElement *getChild(int index);
	std::vector<UiElement*> getChildren();
	void updateDrawData();
	static void updateProjection();
	virtual ~UiElement();
	virtual ShaderType getShaderType() = 0;
	static glm::mat4 getProjection();
	Transform worldTransform;
	InterfaceTransform localTransform;
	void setActive(bool active);
	bool isActive();
	void setRotationAnchor(UiAnchor anchor);
	static glm::vec3 referenceRescaler;
protected:
	UiElement *parent = nullptr;
	std::vector<UiElement*> children;
	virtual void updateWorld();
	bool active = true;
	static glm::mat4 projection;
	static glm::mat4 view;
	static glm::vec2 createScaledSize(float width, float height);
	glm::vec2 createSizeScaledByWidth(float size);
	glm::vec2 createSizeScaledByHeight(float size);
	glm::vec2 getRescaledSize();
	glm::vec2 getRescaledPosition();
	glm::vec2 getRescaledModeledSize();
	glm::vec2 getRescaledModeledPosition();
	float opacity = 1.0f;
	virtual void setup();
	bool dirty = true;
	glm::mat4 worldTransformWithoutRotationScale;
	glm::vec2 modeledPosition{};
	glm::vec2 size{};
	glm::vec2 modeledSize{};
	GLuint vao, vbo = 0;
	UiAnchor anchor, rotationAnchor;
};