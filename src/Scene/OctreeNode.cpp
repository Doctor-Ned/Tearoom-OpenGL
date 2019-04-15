#include "OctreeNode.h"
#include "GraphNode.h"
#include "Components/Collider.h"
#include <iostream>
#include "CollisionSystem.h"
#include "Render/Camera.h"

std::set<GraphNode*> OctreeNode::toInsert2 = std::set<GraphNode*>();
std::vector<GraphNode*> OctreeNode::frustumContainer = std::vector<GraphNode*>();

void OctreeNode::Calculate()
{
	if(gameObjects.size() <= 1)
	{
		return;
	}

	if (glm::distance(boxPos.maxPos.x, boxPos.minPos.x) <= 1.0f) //only x because it's a box
	{
		return;
	}

	std::vector<Box> boxes(8);
	divideSpace(boxes);
	std::vector<std::vector<GraphNode*>> octList(8);
	std::vector<GraphNode*> deList;

	for (GraphNode* gameObject : gameObjects)
	{
		for (int i = 0; i < boxes.size(); i++)
		{
			Collider* collider = gameObject->getComponent<Collider>();
			if(collider == nullptr)
			{
				if (containTest(gameObject->worldTransform.getPosition(), boxes[i]))
				{
					octList[i].push_back(gameObject);
					deList.push_back(gameObject);
					break;
				}
			}
			else
			{
				bool contain = CollisionSystem::getInstance()->containTest(boxes[i].minPos, boxes[i].maxPos, collider);
				if (contain)
				{
					octList[i].push_back(gameObject);
					deList.push_back(gameObject);
					break;
				}
			}
		}
	}

	for(auto& toErase : deList)
	{
		auto i = std::find(gameObjects.begin(), gameObjects.end(), toErase);
		if(i != gameObjects.end())
		{
			gameObjects.erase(i);
		}
	}
	

	for(int i = 0; i < 8; i++)
	{
		if(!octList[i].empty())
		{
			nodes.push_back(std::make_shared<OctreeNode>(boxes[i], this, octList[i]));
		}
	}

	for (auto& child : nodes)
	{
		if(!child->gameObjects.empty())
			child->Calculate();
	}
}

void OctreeNode::CollisionTests(std::vector<GraphNode*> objectsWithColliders)
{
	std::vector<GraphNode*> thisNodeObjectsWithColliders;
	for(GraphNode* object :gameObjects)
	{
		Collider* collider = object->getComponent<Collider>();
		if(collider != nullptr)
		{
			thisNodeObjectsWithColliders.push_back(object);
		}
	}

	for(int i = 0; i < thisNodeObjectsWithColliders.size(); i++)
	{
		Collider* collider1 = thisNodeObjectsWithColliders[i]->getComponent<Collider>();
		for(int j = i + 1; j < thisNodeObjectsWithColliders.size(); j++)
		{
			Collider* collider2 = thisNodeObjectsWithColliders[j]->getComponent<Collider>();
			CollisionSystem::getInstance()->checkCollision(collider1, collider2);
		}
	}

	for (GraphNode* thisNodeObject : thisNodeObjectsWithColliders)
	{
		Collider* collider1 = thisNodeObject->getComponent<Collider>();
		for (GraphNode* NodeObject : objectsWithColliders)
		{
			Collider* collider2 = NodeObject->getComponent<Collider>();
			CollisionSystem::getInstance()->checkCollision(collider1, collider2);
		}
	}

	for(GraphNode* object : thisNodeObjectsWithColliders)
	{
		objectsWithColliders.push_back(object);
	}

	for(auto& node : nodes)
	{
		node->CollisionTests(objectsWithColliders);
	}
}

std::vector<std::shared_ptr<OctreeNode>>& OctreeNode::getNodes()
{
	return nodes;
}

std::vector<GraphNode*>& OctreeNode::getGameObjects()
{
	return gameObjects;
}

Box& OctreeNode::getBox()
{
	return boxPos;
}

void OctreeNode::frustumCulling(Frustum& frustum)
{
	for(GraphNode* gameObject: gameObjects)
	{
		Collider* collider = gameObject->getComponent<Collider>();
		if(collider != nullptr)
		{
			if(collider->getType() == BoxCol)
			{
				if(frustum.boxInFrustum(dynamic_cast<BoxCollider*>(collider)))
				{
					frustumContainer.push_back(gameObject);
				}
			}
			else if(collider->getType() == SphereCol)
			{
				if (frustum.sphereInFrustum(dynamic_cast<SphereCollider*>(collider)))
				{
					frustumContainer.push_back(gameObject);
				}
			}
		}
		else
		{
			if(frustum.pointInFrustum(gameObject->worldTransform.getPosition()))
			{
				frustumContainer.push_back(gameObject);
			}
		}
	}

	for(auto& node : nodes)
	{
		node->frustumCulling(frustum);
	}
}

bool OctreeNode::containTest(glm::vec3& point, Box& box)
{
	for(int i = 0; i < 3; i++)
	{
		if(point[i] <= box.minPos[i] || point[i] >= box.maxPos[i])
		{
			return false;
		}
	}
	return true;
}

void OctreeNode::divideSpace(std::vector<Box>& boxes)
{
	glm::vec3 min, max, middle;
	
	min = boxPos.minPos;
	max = boxPos.middle;
	middle = (min + max) / 2.0f;
	boxes[0] = Box{ min, max, middle };
	
	min = glm::vec3(boxPos.middle.x, boxPos.minPos.y, boxPos.minPos.z);
	max = glm::vec3(boxPos.maxPos.x, boxPos.middle.y, boxPos.middle.z);
	middle = (min + max) / 2.0f;	
	boxes[1] = Box{ min, max, middle };

	min = glm::vec3(boxPos.middle.x, boxPos.minPos.y, boxPos.middle.z);
	max = glm::vec3(boxPos.maxPos.x, boxPos.middle.y, boxPos.maxPos.z);
	middle = (min + max) / 2.0f;
	boxes[2] = Box{ min, max, middle };

	min = glm::vec3(boxPos.minPos.x, boxPos.minPos.y, boxPos.middle.z);
	max = glm::vec3(boxPos.middle.x, boxPos.middle.y, boxPos.maxPos.z);
	middle = (min + max) / 2.0f;
	boxes[3] = Box{ min, max, middle };

	min = glm::vec3(boxPos.minPos.x, boxPos.middle.y, boxPos.minPos.z);
	max = glm::vec3(boxPos.middle.x, boxPos.maxPos.y, boxPos.middle.z);
	middle = (min + max) / 2.0f;
	boxes[4] = Box{ min, max, middle };

	min = glm::vec3(boxPos.middle.x, boxPos.middle.y, boxPos.minPos.z);
	max = glm::vec3(boxPos.maxPos.x, boxPos.maxPos.y, boxPos.middle.z);
	middle = (min + max) / 2.0f;
	boxes[5] = Box{ min, max, middle };

	min = boxPos.middle;
	max = boxPos.maxPos;
	middle = (min + max) / 2.0f;
	boxes[6] = Box{ min, max, middle };

	min = glm::vec3(boxPos.minPos.x, boxPos.middle.y, boxPos.middle.z);
	max = glm::vec3(boxPos.middle.x, boxPos.maxPos.y, boxPos.maxPos.z);
	middle = (min + max) / 2.0f;
	boxes[7] = Box{ min, max, middle };
}

void OctreeNode::draw()
{
	if (mesh_ptr != nullptr)
		//todo: replace this with an implementation of Renderable interface so that it's properly managed by the scene
		// though. rendering Octree is a debug-only thing, we won't do it in the game releases so i guess it doesn't matter
		AssetManager::getInstance()->getShader(STColor)->use();
		mesh_ptr->draw(AssetManager::getInstance()->getShader(STColor), glm::mat4(1));
	for(auto& octree : nodes)
	{
			octree->draw();
	}
}

std::shared_ptr<OctreeNode>& OctreeNode::getInstance()
{
	static std::shared_ptr<OctreeNode> octreeRoot;
	if (octreeRoot == nullptr)
	{
		octreeRoot = std::make_shared<OctreeNode>();
	}
	return octreeRoot;
}

void OctreeNode::RebuildTree(float dimension)
{
	gameObjects.clear();
	nodes.clear();
	frustumContainer.clear();
	boxPos.maxPos = glm::vec3(dimension);
	boxPos.minPos = glm::vec3(-dimension);
	boxPos.middle = (boxPos.maxPos + boxPos.minPos) / 2.0f;
	mesh_ptr = std::make_shared<MeshColorBox>(glm::vec3(-dimension), glm::vec3(dimension), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mesh_ptr->setRenderMode(GL_LINES);
	mesh_ptr->setUseLight(false);
	for (GraphNode* graphNode : toInsert2)
	{
		gameObjects.push_back(graphNode);
	}
	toInsert2.clear();
}

OctreeNode::OctreeNode(float dimension)
{
	boxPos.maxPos = glm::vec3(dimension);
	boxPos.minPos = glm::vec3(-dimension);
	boxPos.middle = (boxPos.maxPos + boxPos.minPos) / 2.0f;
	mesh_ptr = std::make_shared<MeshColorBox>(glm::vec3(-dimension), glm::vec3(dimension), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mesh_ptr->setRenderMode(GL_LINES);
	mesh_ptr->setUseLight(false);
	for(GraphNode* graphNode : toInsert2)
	{
		gameObjects.push_back(graphNode);
	}
	toInsert2.clear();
}

OctreeNode::OctreeNode(Box _box, OctreeNode* _parent, std::vector<GraphNode*> _gameObjects) 
	: boxPos(_box), parent(_parent), gameObjects(_gameObjects)
{

	mesh_ptr = std::make_shared<MeshColorBox>(boxPos.minPos, boxPos.maxPos, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mesh_ptr->setRenderMode(GL_LINES);
	mesh_ptr->setUseLight(false);
}

OctreeNode::OctreeNode()
{
	
}

OctreeNode::~OctreeNode()
{

}

GraphNode* OctreeNode::findObjectByRayPoint(const glm::vec3& rayPos, static std::shared_ptr<OctreeNode>& node)
{
	if (containTest(glm::vec3(rayPos), node->boxPos)) 
	{
		for (GraphNode* game_object : node->gameObjects) {
			Collider* collider = game_object->getComponent<Collider>();
			if (collider != nullptr)
			{
				if (CollisionSystem::getInstance()->containTest(rayPos, collider))
				{
					return game_object;
				}
			}
		}
		for (auto& octreeNode : node->nodes) 
		{
			GraphNode* gameObject = findObjectByRayPoint(rayPos, octreeNode);
			if (gameObject != nullptr) {
				return gameObject;
			}
		}
	}
	return nullptr;
}