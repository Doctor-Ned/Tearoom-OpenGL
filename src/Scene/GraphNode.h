#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include "Global.h"
#include "Transform.h"
#include "Components/Component.h"
#include "GuiConfigurable.h"
#include "Render/Renderable.h"
#include <queue>
#include "Serialization/Serializable.h"
#include "ComposedTransform.h"
class Mesh;
class Shader;

class GraphNode final : public GuiConfigurable, public Renderable, public Serializable {
public:
	Transform worldTransform; //Transform has address to the dirty flag and can change it.
	ComposedTransform localTransform;
	GraphNode(Mesh* mesh = nullptr, GraphNode* parent = nullptr);
	void updateDrawData() override;
	void drawSelf(Shader *shader) override;
	ShaderType getShaderType() override;
	void update(double timeDiff);
	GraphNode *getParent() const;
	void setParent(GraphNode *parent);
	void addChild(GraphNode* child);
	void removeChild(GraphNode* child);
	void addComponent(Component* component);
	void move(glm::vec3 direction);
	void moveRelative(glm::vec3 direction, bool allowVertical = false);
	void moveRelative(float forward, float right, float up);
	void moveForward(float distance, bool allowVertical = false);
	void moveBackward(float distance, bool allowVertical = false);
	void moveLeft(float distance);
	void moveRight(float distance);
	void moveUp(float distance);
	void moveDown(float distance);
	glm::vec3 getUpVector();
	glm::vec3 getDownVector();
	glm::vec3 getFrontVector();
	glm::vec3 getBackVector();
	glm::vec3 getRightVector();
	glm::vec3 getLeftVector();
	glm::vec3 getRelative(glm::vec3 base);
	template <class T> void removeComponent(T *component);
	template <class T> void removeComponents();
	template <class T> T* getComponent();
	template <class T> std::vector<T*> getComponents();
	template <class T> T* getComponentInChildren();
	std::vector<Component*> getComponents();
	GraphNode* getChild(int index);
	float getOpacity();
	void setOpacity(float opacity);
	Mesh* getMesh();
	~GraphNode();
	bool isOpaque() override;
	bool isActive() override;
	void setActive(bool active);
	const std::string& getName() const;
	void setName(std::string&& name);
	std::vector<GraphNode*> getChildren() const;
	std::vector<Renderable*> getRenderableComponents() const;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer *serializer) override;
	SerializableType getSerializableType() override;
	void setTempRenderMode(GLenum tempRenderMode);
	void removeTempRenderMode();
	glm::vec3 getPosition();
protected:
	GLenum tempRenderMode = GL_NONE;
	void updateWorld();
	std::string name;
	void renderGui() override;
	bool active = true;
	std::vector<GraphNode*> children;
	std::vector<Component*> components;
	std::vector<Renderable*> renderableComponents;
	GraphNode* parent;
	Mesh* mesh = nullptr;
	bool dirty;
};

template <class T>
void GraphNode::removeComponent(T* component) {
	for (std::vector<Component*>::iterator i = components.begin(); i != components.end();) {
		if (*i == component) {
			Renderable *r = dynamic_cast<Renderable*>(*i);
			if (r) {
				for (auto j = renderableComponents.begin(); j != renderableComponents.end();) {
					if (*j == r) {
						renderableComponents.erase(j);
						break;
					}
					++j;
				}
			}
			i = components.erase(i);
			break;
		}
		++i;
	}
}

template <class T>
void GraphNode::removeComponents() {
	for (std::vector<Component*>::iterator i = components.begin(); i != components.end();) {
		if (dynamic_cast<T*>(*i)) {
			components.erase(i);
			Renderable *r = dynamic_cast<Renderable*>(*i);
			if (r) {
				for (auto j = renderableComponents.begin(); j != renderableComponents.end();) {
					if (*j == r) {
						renderableComponents.erase(j);
						break;
					}
					++j;
				}
			}
		} else {
			++i;
		}
	}
}

template <class T> T* GraphNode::getComponent() {
	for (auto &comp : components) {
		T* result = dynamic_cast<T*>(comp);
		if (result) {
			return result;
		}
	}
	return nullptr;
}

template <class T>
std::vector<T*> GraphNode::getComponents() {
	std::vector<T*> result;
	for (auto &comp : components) {
		T* component = dynamic_cast<T*>(comp);
		if (component) {
			result.push_back(component);
		}
	}
	return result;
}

template <class T>
T* GraphNode::getComponentInChildren() {
	T* t = getComponent<T>();
	if(t) {
		return t;
	}
	if(children.empty()) {
		return nullptr;
	}
	for(auto &child : children) {
		t = child->getComponentInChildren<T>();
		if(t != nullptr) {
			return t;
		}
	}
	return nullptr;
}

#endif
