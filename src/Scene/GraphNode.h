#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include "Headers.h"
#include "Mesh/Mesh.h"

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
	glm::mat4 getLocal();
	glm::mat4 getWorld();
	GraphNode *getParent();
	void setScale(float scale);
	void setLocal(glm::mat4 local);
	void addChild(GraphNode* child);
	void removeChild(GraphNode* child);
	Mesh* getMesh();
protected:
	glm::mat4 local, world;
	std::vector<GraphNode*> children;
	GraphNode* parent;
	Mesh* mesh;
	bool dirty;
	float scale;
};

#endif
