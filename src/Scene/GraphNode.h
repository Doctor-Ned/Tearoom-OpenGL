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
	//glm::mat4 getLocal() const;
	//glm::mat4 getWorld() const;
	GraphNode *getParent() const;
	void setParent(GraphNode *parent, bool preserveWorldPosition = false);
	void setScale(float scale);
	//void setLocal(glm::mat4 local);
	void addChild(GraphNode* child);
	void removeChild(GraphNode* child);
	void addComponent(Component*component);
	void removeComponent(int index);
	template <class T> Component* getComponent();
	GraphNode* getChild(int index);
	Mesh* getMesh();
	virtual ~GraphNode();
protected:
	//glm::mat4 local, world;
	std::vector<GraphNode*> children;
	std::vector<Component*> components;
	GraphNode* parent;
	Mesh* mesh;
	bool dirty;
	float scale;
};

template <class T> Component* GraphNode::getComponent() {
	for (auto &comp : components) {
		Component* result = dynamic_cast<T*>(comp);
		if (result) {
			return result;
		}
	}
	return nullptr;
}

#endif
