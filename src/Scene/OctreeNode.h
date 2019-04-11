#ifndef OCTREE_NODE_H
#define OCTREE_NODE_H
#include <vector>
#include <queue>
#include <glm/glm.hpp>
#include "Mesh/MeshColorBox.h"

struct Box
{
	glm::vec3 minPos;
	glm::vec3 maxPos;
	glm::vec3 middle;
};
class GraphNode;
class OctreeNode
{
private:
	std::vector<OctreeNode*> nodes;
	std::vector<GraphNode*> gameObjects;
	OctreeNode* parent;
	MeshColorBox* mesh2;
	char bitmask = 0;
	Box boxPos;
	inline void divideSpace(std::vector<Box>& boxes);
	inline bool containTest(glm::vec3& point, Box& box);
public:
	static std::vector<GraphNode*> toInsert;
	OctreeNode(glm::vec3 _minPos, glm::vec3 _maxPos);
	OctreeNode(Box _box, OctreeNode* parrent, std::vector<GraphNode*> _gameObjects);
	OctreeNode();
	~OctreeNode();
	void draw();
	void Calculate();
	void CollisionTests(std::vector<GraphNode*> objectsWithColliders = std::vector<GraphNode*>());
};
#endif
