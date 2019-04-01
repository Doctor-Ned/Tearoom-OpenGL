#ifndef OCTREE_NODE_H
#define OCTREE_NODE_H
#include <vector>
#include <queue>
#include <glm/vec3.hpp>
#include "GraphNode.h"
#include "Mesh/MeshColorBox.h"
struct Box
{
	glm::vec3 minPos;
	glm::vec3 maxPos;
	glm::vec3 middle;
};

class OctreeNode
{
private:
	std::vector<OctreeNode*> children;
	std::vector<GraphNode*> gameObjects;
	OctreeNode* parent;
	GraphNode* box;
	Box boxPos;
	glm::vec4 color;
	inline void divideSpace(std::vector<Box>& boxes);
public:
	static std::queue<GraphNode*> toInsert;
	OctreeNode(glm::vec3 _minPos, glm::vec3 _maxPos);
	OctreeNode(Box _box, OctreeNode* parrent);
	OctreeNode();
	~OctreeNode();
	void draw();
	void Calculate();
};
#endif
