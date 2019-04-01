#include "OctreeNode.h"
#include <iostream>

void OctreeNode::Calculate()
{
	if (glm::distance(boxPos.maxPos.x, boxPos.minPos.x) <= 2.5f)
	{
		return;
	}

	std::vector<Box> boxes;
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
	//boxes[0] = { min, max, middle };
	boxes.push_back(Box{ min, max, middle });
	
	min = glm::vec3(boxPos.middle.x, boxPos.minPos.y, boxPos.minPos.z);
	max = glm::vec3(boxPos.maxPos.x, boxPos.middle.y, boxPos.middle.z);
	middle = (min + max) / 2.0f;	
	//boxes[1] = { min, max, middle };
	boxes.push_back(Box{ min, max, middle });

	min = glm::vec3(boxPos.middle.x, boxPos.minPos.y, boxPos.middle.z);
	max = glm::vec3(boxPos.maxPos.x, boxPos.middle.y, boxPos.maxPos.z);
	middle = (min + max) / 2.0f;
	//boxes[2] = { min, max, middle };
	boxes.push_back(Box{ min, max, middle });

	min = glm::vec3(boxPos.minPos.x, boxPos.minPos.y, boxPos.middle.z);
	max = glm::vec3(boxPos.middle.x, boxPos.middle.y, boxPos.maxPos.z);
	middle = (min + max) / 2.0f;
	//boxes[3] = { min, max, middle };
	boxes.push_back(Box{ min, max, middle });

	min = glm::vec3(boxPos.minPos.x, boxPos.middle.y, boxPos.minPos.z);
	max = glm::vec3(boxPos.middle.x, boxPos.maxPos.y, boxPos.middle.z);
	middle = (min + max) / 2.0f;
	//boxes[4] = { min, max, middle };
	boxes.push_back(Box{ min, max, middle });

	min = glm::vec3(boxPos.middle.x, boxPos.middle.y, boxPos.minPos.z);
	max = glm::vec3(boxPos.maxPos.x, boxPos.maxPos.y, boxPos.middle.z);
	middle = (min + max) / 2.0f;
	//boxes[5] = { min, max, middle };
	boxes.push_back(Box{ min, max, middle });

	min = boxPos.middle;
	max = boxPos.maxPos;
	middle = (min + max) / 2.0f;
	//boxes[6] = { min, max, middle };
	boxes.push_back(Box{ min, max, middle });

	min = glm::vec3(boxPos.minPos.x, boxPos.middle.y, boxPos.middle.z);
	max = glm::vec3(boxPos.middle.x, boxPos.maxPos.y, boxPos.maxPos.z);
	middle = (min + max) / 2.0f;
	//boxes[7] = { min, max, middle };
	boxes.push_back(Box{ min, max, middle });

	//octant[0] = new BoundingBox(m_region.Min, center);
	//octant[1] = new BoundingBox(new Vector3(center.X, m_region.Min.Y, m_region.Min.Z), new Vector3(m_region.Max.X, center.Y, center.Z));
	//octant[2] = new BoundingBox(new Vector3(center.X, m_region.Min.Y, center.Z), new Vector3(m_region.Max.X, center.Y, m_region.Max.Z));
	//octant[3] = new BoundingBox(new Vector3(m_region.Min.X, m_region.Min.Y, center.Z), new Vector3(center.X, center.Y, m_region.Max.Z));
	//octant[4] = new BoundingBox(new Vector3(m_region.Min.X, center.Y, m_region.Min.Z), new Vector3(center.X, m_region.Max.Y, center.Z));
	//octant[5] = new BoundingBox(new Vector3(center.X, center.Y, m_region.Min.Z), new Vector3(m_region.Max.X, m_region.Max.Y, center.Z));
	//octant[6] = new BoundingBox(center, m_region.Max);
	//octant[7] = new BoundingBox(new Vector3(m_region.Min.X, center.Y, center.Z), new Vector3(center.X, m_region.Max.Y, m_region.Max.Z));
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