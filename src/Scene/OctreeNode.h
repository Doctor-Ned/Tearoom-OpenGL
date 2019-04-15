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

struct Frustum;
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
public:
	static std::shared_ptr<OctreeNode>& getInstance();
	static GraphNode* findObjectByRayPoint(const glm::vec3& rayPos, static std::shared_ptr<OctreeNode>& node);
	static std::vector<GraphNode*> frustumContainer;
	static std::set<GraphNode*> toInsert2;
	static inline bool containTest(glm::vec3& point, Box& box);
	void RebuildTree(float dimension);
	OctreeNode(Box _box, OctreeNode* parrent, std::vector<GraphNode*> _gameObjects);
	OctreeNode();
	~OctreeNode();
	void draw();
	void Calculate();
	void CollisionTests(std::vector<GraphNode*> objectsWithColliders = std::vector<GraphNode*>());
	std::vector<std::shared_ptr<OctreeNode>>& getNodes();
	std::vector<GraphNode*>& getGameObjects();
	Box& getBox();
	void frustumCulling(Frustum& frustum);
};
#endif
