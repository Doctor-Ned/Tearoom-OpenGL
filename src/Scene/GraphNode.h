#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include "Global.h"

class Mesh;
class Shader;

class GraphNode {
public:
	GraphNode(Mesh* mesh = nullptr, GraphNode* parent = nullptr);
	virtual void draw();
	virtual void draw(GraphNode *excluded);
	virtual void draw(std::vector<GraphNode*> excluded);
	virtual void draw(Shader *shader);
	virtual void draw(Shader *shader, GraphNode *excluded);
	virtual void draw(Shader *shader, std::vector<GraphNode*> excluded);
	virtual void update(double timeDiff);
	glm::mat4 getLocal() const;
	glm::mat4 getWorld() const;
	GraphNode *getParent() const;
	void setParent(GraphNode *parent, bool preserveWorldPosition = false);
	void setScale(float scale);
	void setLocal(glm::mat4 local);
	void addChild(GraphNode* child);
	void removeChild(GraphNode* child);
	Mesh* getMesh();
	virtual ~GraphNode();
protected:
	glm::mat4 local, world;
	std::vector<GraphNode*> children;
	GraphNode* parent;
	Mesh* mesh;
	bool dirty;
	float scale;
};

#endif
