#include "OctreeNode.h"
#include <iostream>

std::queue<GraphNode*> OctreeNode::toInsert = std::queue<GraphNode*>();

void OctreeNode::Calculate()
{
	if(gameObjects.size() <= 1)
	{
		return;
	}

	if (glm::distance(boxPos.maxPos.x, boxPos.minPos.x) <= 2.5f) //only x because it's a box
	{
		return;
	}

	std::vector<Box> boxes(8);
	divideSpace(boxes);
	for (int i = 0; i < boxes.size(); i++)
	{
		children.push_back(new OctreeNode(boxes[i], this));
	}

	for (OctreeNode* child : children)
	{
		child->Calculate();
	}
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

OctreeNode::~OctreeNode()
{
	delete box;
	for(OctreeNode* child: children)
	{
		delete child;
	}
}

void OctreeNode::draw()
{
	box->draw();
	for(OctreeNode* octree: children)
	{
		octree->draw();
	}
}

OctreeNode::OctreeNode(glm::vec3 _minPos, glm::vec3 _maxPos)
{
	float maxVal = _maxPos.x;
	maxVal > _maxPos.y ? maxVal = _maxPos.y : maxVal;
	maxVal > _maxPos.z ? maxVal = _maxPos.z : maxVal;
	maxVal > glm::abs(_minPos.x) ? maxVal = glm::abs(_minPos.x) : maxVal;
	maxVal > glm::abs(_minPos.y) ? maxVal = glm::abs(_minPos.y) : maxVal;
	maxVal > glm::abs(_minPos.z) ? maxVal = glm::abs(_minPos.z) : maxVal;
	
	boxPos.maxPos = glm::vec3(maxVal, maxVal, maxVal);
	boxPos.minPos = glm::vec3(maxVal * (-1), maxVal * (-1), maxVal * (-1));
	boxPos.middle = (boxPos.maxPos + boxPos.minPos) / 2.0f;

	color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	MeshColorBox* tmp = new MeshColorBox(boxPos.minPos, boxPos.maxPos, color);
	tmp->setUseLight(false);
	
	box = new GraphNode(tmp);

	while(toInsert.empty())
	{
		gameObjects.push_back(toInsert.front());
		toInsert.pop();
	}
}

OctreeNode::OctreeNode(Box _box, OctreeNode* _parent) : boxPos(_box), parent(_parent)
{
	color = parent->color / 2.0f;
	MeshColorBox* tmp = new MeshColorBox(boxPos.minPos, boxPos.maxPos, color);
	tmp->setUseLight(false);
	box = new GraphNode(tmp);
}

OctreeNode::OctreeNode()
{

}