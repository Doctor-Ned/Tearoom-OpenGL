#ifndef BONE_H
#define BONE_H
#include "glm/glm.hpp"
#include <iostream>
#include <map>
#include "Scene/Transform.h"
#include "Scene/ComposedTransform.h"

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

	Bone() : worldTransform(Transform(dirty)), localTransform(ComposedTransform(dirty))
	{

	};

	Bone(Bone* b) : worldTransform(Transform(dirty)), localTransform(ComposedTransform(dirty))
	{
		ID = b->ID;
		name = b->name;
		worldTransform.setMatrix( b->worldTransform.getMatrix() );
		localTransform.setData(b->localTransform.getData());
		inverseBindPose = b->inverseBindPose;
		transform = b->transform;
		verticesWages = b->verticesWages;
		meshIndex = b->meshIndex;

		for(auto child : b->children)
		{
			children.push_back(new Bone(child));
		}
	}

	void setParentsForChildren(Bone* _parent = nullptr)
	{
		parent = _parent;
		for (auto child : children)
		{
			child->setParentsForChildren(this);
		}
	}

	void updateWorld()
	{
		if (parent != nullptr) {
			worldTransform.setMatrix(parent->worldTransform.getMatrix() * localTransform.getMatrix());
		}
		else {
			worldTransform.setMatrix(localTransform.getMatrix());
		}

		for (auto &child : children) {
			child->updateWorld();
		}
	}

	void toString(int wciecie)
	{
		std::cout << name << " ID: " << ID << std::endl;
		wciecie++;
		for(auto& child : children)
		{
			for(int i = 0; i < wciecie; i ++)
			{
				std::cout << "  ";
			}
			child->toString(wciecie);
		}
	}

	void setID(bool init = true)
	{
		static int id = 0;
		if(init)
		{
			id = 0;
		}
		
		{
			ID = id;
			for (auto child : children)
			{
				id++;
				child->setID(false);
			}
		}
	}
};

#endif