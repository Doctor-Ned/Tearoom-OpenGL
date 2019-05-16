#ifndef BONE_H
#define BONE_H
#include "glm/glm.hpp"
#include <iostream>
#include <map>
#include "Scene/Transform.h"
#include "Scene/ComposedTransform.h"
#include <vector>

class Bone {
public:
	unsigned int ID;
	std::string name;
	Bone* parent;
	std::vector<Bone*> children;
	glm::mat4 inverseBindPose;
	glm::mat4 transform;
	Transform worldTransform;
	ComposedTransform localTransform;
	std::map<unsigned int, float> verticesWages;
	int meshIndex;
	bool dirty = true;
	Bone();
	Bone(Bone* b);
	void setParentsForChildren(Bone* _parent = nullptr);
	void updateWorld();
	void toString(int wciecie);
	void setID(bool init = true);
};

#endif