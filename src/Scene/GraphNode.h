#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include "Global.h"
#include "Transform.h"
#include "Components/Component.h"
class Mesh;
class Shader;

class GraphNode {
public:
	Transform worldTransform; //Transform has addres to dirty flag and can change it.
	Transform localTransform;
	GraphNode(Mesh* mesh = nullptr, GraphNode* parent = nullptr);
	void draw();
	void draw(GraphNode *excluded);
	void draw(std::vector<GraphNode*> excluded);
	void draw(Shader *shader);
	void draw(Shader *shader, GraphNode *excluded);
	void draw(Shader *shader, std::vector<GraphNode*> excluded);
	virtual void draw(Shader *shader, GraphNode **excluded, int excludedCount);
	virtual void update(double timeDiff);
	GraphNode *getParent() const;
	void setParent(GraphNode *parent, bool preserveWorldPosition = false);
	void addChild(GraphNode* child);
	void removeChild(GraphNode* child);
	void addComponent(Component*component);
	template <class T> void removeComponent(T *component);
	template <class T> void removeComponents();
	template <class T> Component* getComponent();
	template <class T> std::vector<Component*> getComponents();
	GraphNode* getChild(int index);
	Mesh* getMesh();
	virtual ~GraphNode();
protected:
	std::vector<GraphNode*> children;
	std::vector<Component*> components;
	GraphNode* parent;
	Mesh* mesh;
	bool dirty;
};

template <class T>
void GraphNode::removeComponent(T* component) {
	for (std::vector<Component*>::iterator i = components.begin(); i != components.end(); ++i) {
		if (*i == component) {
			i = components.erase(i);
			break;
		}
	}
}

template <class T>
void GraphNode::removeComponents() {
	for (std::vector<Component*>::iterator i = components.begin(); i != components.end();) {
		if (dynamic_cast<T*>(*i)) {
			components.erase(i);
		} else {
			++i;
		}
	}
}

template <class T> Component* GraphNode::getComponent() {
	for (auto &comp : components) {
		Component* result = dynamic_cast<T*>(comp);
		if (result) {
			return result;
		}
	}
	return nullptr;
}

template <class T>
std::vector<Component*> GraphNode::getComponents() {
	std::vector<Component*> result;
	for (auto &comp : components) {
		Component* component = dynamic_cast<T*>(comp);
		if (component) {
			result.push_back(component);
		}
	}
	return result;
}

#endif
