#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include "Global.h"
#include "Transform.h"
#include "Components/Component.h"
#include "GuiConfigurable.h"
#include "Render/Renderable.h"
class Mesh;
class Shader;

class GraphNode : public GuiConfigurable, public Renderable {
public:
	Transform worldTransform; //Transform has addres to dirty flag and can change it.
	Transform localTransform;
	GraphNode(Mesh* mesh = nullptr, GraphNode* parent = nullptr);
	void updateDrawData() override;
	void drawSelf(Shader *shader) override;
	ShaderType getShaderType() override;
	virtual void update(double timeDiff);
	GraphNode *getParent() const;
	void setParent(GraphNode *parent, bool preserveWorldPosition = false);
	void addChild(GraphNode* child);
	void removeChild(GraphNode* child);
	void addComponent(Component* component);
	template <class T> void removeComponent(T *component);
	template <class T> void removeComponents();
	template <class T> T* getComponent();
	template <class T> std::vector<T*> getComponents();
	std::vector<Component*> getComponents();
	GraphNode* getChild(int index);
	float getOpacity();
	void setOpacity(float opacity);
	Mesh* getMesh();
	virtual ~GraphNode();
	bool isOpaque() override;
	bool isActive() const;
	void setActive(bool active);
	const char* getName() const;
	void setName(const char* name);
	std::vector<GraphNode*> getChildren() const;
	std::vector<Renderable*> getRenderableComponents() const;
protected:
	virtual void updateWorld();
	const char* name;
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
	for (std::vector<Component*>::iterator i = components.begin(); i != components.end(); ++i) {
		if (*i == component) {
			i = components.erase(i);
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
			break;
		}
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

#endif
