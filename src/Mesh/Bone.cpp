#include "Bone.h"
#include "Render/Shader.h"

Bone::Bone() : worldTransform(Transform(dirty)), localTransform(ComposedTransform(dirty)) { };

Bone::Bone(Bone* b) : worldTransform(Transform(dirty)), localTransform(ComposedTransform(dirty)) {
	ID = b->ID;
	name = b->name;
	worldTransform.setMatrix(b->worldTransform.getMatrix());
	localTransform.setData(b->localTransform.getData());
	inverseBindPose = b->inverseBindPose;
	transform = b->transform;
	//verticesWages = b->verticesWages;
	//meshIndex = b->meshIndex;

	for (auto child : b->children) {
		children.push_back(new Bone(child));
	}
}

void Bone::setParentsForChildren(Bone* _parent) {
	parent = _parent;
	for (auto child : children) {
		child->setParentsForChildren(this);
	}
}

void Bone::updateWorld() {
	if (parent != nullptr) {
		worldTransform.setMatrix(parent->worldTransform.getMatrix() * localTransform.getMatrixQuat());
	} else {
		worldTransform.setMatrix(localTransform.getMatrixQuat());
	}

	for (auto &child : children) {
		child->updateWorld();
	}
}

void Bone::toString(int wciecie) {
	std::cout << name.c_str() << " ID: " << ID << std::endl;
	wciecie++;
	for (auto& child : children) {
		for (int i = 0; i < wciecie; i++) {
			std::cout << "  ";
		}
		child->toString(wciecie);
	}
}

void Bone::setID(bool init) {
	static int id = 0;
	if (init) {
		id = 0;
	}

	{
		ID = id;
		for (auto child : children) {
			id++;
			child->setID(false);
		}
	}
	if(ID > MAX_BONE_TRANSFORMS) {
		throw std::exception("MAX_BONE_TRANSFORMS was lower than the ID of current bone!");
	}
}