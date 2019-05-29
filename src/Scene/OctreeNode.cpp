#include "OctreeNode.h"
#include "GraphNode.h"
#include "Components/Collider.h"
#include <iostream>
#include "CollisionSystem.h"
#include "Components/Camera.h"

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
			auto colliders = gameObject->getComponents<Collider>();
			if(colliders.empty())
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
				std::vector<bool> collidersAreInBox(colliders.size());
				for(Collider* collider : colliders)
				{
					collidersAreInBox.push_back(CollisionSystem::getInstance()->containTest(boxes[i].minPos, boxes[i].maxPos, collider));
				}
				unsigned int colliderInBoxCounter = 0;
				for(int i = 0; i < collidersAreInBox.size(); i++)
				{
					if(collidersAreInBox[i] == true)
					{
						colliderInBoxCounter++;
					}
				}
				bool canPutIntoChildNode = colliderInBoxCounter == collidersAreInBox.size();
				if (canPutIntoChildNode)
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
			nodes.emplace_back(OctreeNode( boxes[i], this, octList[i] ));
		}
	}

	for (auto& child : nodes)
	{
		if(!child.gameObjects.empty())
			child.Calculate();
	}
}

void OctreeNode::CollisionTests(std::vector<GraphNode*> objectsWithColliders)
{
	std::vector<GraphNode*> thisNodeObjectsWithColliders;
	for(GraphNode* object :gameObjects)
	{
		auto colliders = object->getComponents<Collider>();
		if(!colliders.empty())
		{
			thisNodeObjectsWithColliders.push_back(object);
		}
	}

	for(int i = 0; i < thisNodeObjectsWithColliders.size(); i++)
	{
		auto colliders1 = thisNodeObjectsWithColliders[i]->getComponents<Collider>();
		for(int j = i + 1; j < thisNodeObjectsWithColliders.size(); j++)
		{
			auto colliders2 = thisNodeObjectsWithColliders[j]->getComponents<Collider>();
			//
			for(Collider* collider1 : colliders1)
			{
				for (Collider* collider2 : colliders2)
				{
					CollisionSystem::getInstance()->checkCollision(collider1, collider2);
				}
			}
		}
	}

	for (GraphNode* thisNodeObject : thisNodeObjectsWithColliders)
	{
		auto colliders1 = thisNodeObject->getComponents<Collider>();
		for (GraphNode* NodeObject : objectsWithColliders)
		{
			auto colliders2 = NodeObject->getComponents<Collider>();
			for (Collider* collider1 : colliders1)
			{
				for (Collider* collider2 : colliders2)
				{
					CollisionSystem::getInstance()->checkCollision(collider1, collider2);
				}
			}
		}
	}

	for(GraphNode* object : thisNodeObjectsWithColliders)
	{
		objectsWithColliders.push_back(object);
	}

	for(auto& node : nodes)
	{
		node.CollisionTests(objectsWithColliders);
	}
}

std::vector<OctreeNode>& OctreeNode::getNodes()
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
		auto colliders = gameObject->getComponents<Collider>();
		if(!colliders.empty())
		{
			for (Collider* collider : colliders)
			{
				if (collider->getType() == BoxCol)
				{
					if (frustum.boxInFrustum(static_cast<BoxCollider*>(collider)))
					{
						frustumContainer.push_back(gameObject);
					}
				}
				else if (collider->getType() == SphereCol)
				{
					if (frustum.sphereInFrustum(static_cast<SphereCollider*>(collider)))
					{
						frustumContainer.push_back(gameObject);
					}
				}
			}
		}
		else
		{
			//if(frustum.pointInFrustum(gameObject->worldTransform.getPosition()))
			//{
				frustumContainer.push_back(gameObject);
			//}
		}
	}

	for(auto& node : nodes)
	{
		node.frustumCulling(frustum);
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
		mesh_ptr->drawSelf(AssetManager::getInstance()->getShader(STColor), glm::mat4(1));
	for(auto& octree : nodes)
	{
			octree.draw();
	}
}

OctreeNode& OctreeNode::getInstance()
{
	static bool initialized = false;
	static OctreeNode octreeRoot;
	if (!initialized)
	{
		octreeRoot = OctreeNode();
		initialized = true;
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
	/*mesh_ptr = std::make_shared<MeshColorBox>(glm::vec3(-dimension), glm::vec3(dimension), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mesh_ptr->setRenderMode(GL_LINE_STRIP);
	mesh_ptr->setUseLight(false);*/
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
	/*mesh_ptr = std::make_shared<MeshColorBox>(glm::vec3(-dimension), glm::vec3(dimension), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mesh_ptr->setRenderMode(GL_LINE_STRIP);
	mesh_ptr->setUseLight(false);*/
	for(GraphNode* graphNode : toInsert2)
	{
		gameObjects.push_back(graphNode);
	}
	toInsert2.clear();
}

OctreeNode::OctreeNode(Box _box, OctreeNode* _parent, std::vector<GraphNode*> _gameObjects) 
	: boxPos(_box), parent(_parent), gameObjects(_gameObjects)
{

	/*mesh_ptr = std::make_shared<MeshColorBox>(boxPos.minPos, boxPos.maxPos, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mesh_ptr->setRenderMode(GL_LINE_STRIP);
	mesh_ptr->setUseLight(false);*/
}

OctreeNode::OctreeNode()
{
	
}

OctreeNode::~OctreeNode()
{
	//std::cout << "OctreeNode destroyed" << std::endl;
	nodes.clear();
}

GraphNode* OctreeNode::findObjectByRayPoint(const glm::vec3& rayPos, static OctreeNode& node, Collider* toIgnore)
{
	if (containTest(glm::vec3(rayPos), node.boxPos)) 
	{
		for (GraphNode* game_object : node.gameObjects) {
			auto colliders = game_object->getComponents<Collider>();
			if (!colliders.empty())
			{
				for(auto collider : colliders)
				{
					if (collider != toIgnore)
					{
						if (CollisionSystem::getInstance()->containTest(rayPos, collider))
						{
							return game_object;
						}
					}
				}
			}
		}
		for (auto& octreeNode : node.nodes) 
		{
			GraphNode* gameObject = findObjectByRayPoint(rayPos, octreeNode, toIgnore);
			if (gameObject != nullptr) {
				return gameObject;
			}
		}
	}
	return nullptr;
}