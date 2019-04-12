#ifndef OCTREE_NODE_H
#define OCTREE_NODE_H
#include <vector>
#include <queue>
#include <glm/glm.hpp>
#include "Mesh/MeshColorBox.h"
#include <memory>
#include <set>

struct Box
{
	glm::vec3 minPos;
	glm::vec3 maxPos;
	glm::vec3 middle;
};

class GraphNode;
class Camera;
class OctreeNode
{
private:
	std::vector<std::shared_ptr<OctreeNode>> nodes;
	std::vector<GraphNode*> gameObjects;
	OctreeNode* parent;
	std::shared_ptr<MeshColorBox> mesh_ptr;
	Box boxPos;
	OctreeNode(float dimension);
	inline void divideSpace(std::vector<Box>& boxes);
	inline bool containTest(glm::vec3& point, Box& box);
public:
	static std::shared_ptr<OctreeNode>& getInstance();
	static std::set<GraphNode*> toInsert2;
	void RebuildTree(float dimension);
	OctreeNode(Box _box, OctreeNode* parrent, std::vector<GraphNode*> _gameObjects);
	OctreeNode();
	~OctreeNode();
	void draw();
	void Calculate();
	void CollisionTests(std::vector<GraphNode*> objectsWithColliders = std::vector<GraphNode*>());
	GraphNode* castRayFromCamera(Camera* camera, float distance);
};
#endif
