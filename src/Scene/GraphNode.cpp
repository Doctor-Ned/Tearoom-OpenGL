#include "GraphNode.h"
#include "Mesh/Mesh.h"
#include <algorithm>
#include <iostream>

GraphNode::GraphNode(Mesh* mesh, GraphNode* parent) : parent(parent), mesh(mesh), dirty(true), scale(1.0f), localTransform(Transform(dirty)), worldTransform(Transform(dirty)) {
	if (parent != nullptr) {
		parent->addChild(this);
	}
}

void GraphNode::draw() {
	draw(nullptr, nullptr, 0);
}

void GraphNode::draw(GraphNode* excluded) {
	if (excluded != nullptr) {
		draw(nullptr, &excluded, 1);
	} else {
		draw(nullptr, nullptr, 0);
	}
}

void GraphNode::draw(std::vector<GraphNode*> excluded) {
	if (excluded.empty()) {
		draw(nullptr, nullptr, 0);
	} else {
		draw(nullptr, &excluded[0], excluded.size());
	}
}

void GraphNode::draw(Shader* shader) {
	draw(shader, nullptr, 0);
}

void GraphNode::draw(Shader* shader, GraphNode* excluded) {
	if (excluded != nullptr) {
		draw(shader, &excluded, 1);
	} else {
		draw(shader, nullptr, 0);
	}
}

void GraphNode::draw(Shader* shader, std::vector<GraphNode*> excluded) {
	if (excluded.empty()) {
		draw(shader, nullptr, 0);
	} else {
		draw(shader, &excluded[0], excluded.size());
	}
}

void GraphNode::draw(Shader* shader, GraphNode** excluded, int excludedCount) {
	if (parent != nullptr) {
		dirty |= parent->dirty;
		if (dirty) {
			worldTransform.SetMatrix(parent->worldTransform.Matrix() * localTransform.Matrix());
		}
	} else if (dirty) {
		worldTransform.SetMatrix(localTransform.Matrix());
	}

	if (mesh) {
		if (shader == nullptr) {
			mesh->draw(worldTransform.Matrix(), scale);
		} else {
			mesh->draw(shader, worldTransform.Matrix(), scale);
		}
	}

	for (Component* component : components)
	{
		component->draw();
	}

	for (int i = 0; i < children.size(); i++) {
		bool exclude = false;
		for (int j = 0; j < excludedCount; j++) {
			if (children[i] == excluded[j]) {
				exclude = true;
				break;
			}
		}
		if (!exclude) {
			children[i]->draw(shader, excluded, excludedCount);
		}
	}

	dirty = false;
}

void GraphNode::update(double timeDiff) {
	for(Component* component: components)
	{
		component->update();
	}
	for (auto &child : children) {
		child->update(timeDiff);
	}
}

GraphNode* GraphNode::getParent() const {
	return parent;
}

void GraphNode::setParent(GraphNode* parent, bool preserveWorldPosition) {
	if (preserveWorldPosition) {
		//local = (this->parent->getWorld() / parent->getWorld()) * local;
		localTransform.SetMatrix(
			(this->parent->worldTransform.Matrix() / parent->worldTransform.Matrix()
				* localTransform.Matrix()));
	}
	this->parent->removeChild(this);
	parent->addChild(this);
	dirty = true;
}

void GraphNode::setScale(float scale) {
	this->scale = scale;
}

void GraphNode::addChild(GraphNode* child) {
	child->parent = this;
	if (std::find(children.begin(), children.end(), child) == children.end()) {
		children.push_back(child);
	}
}

void GraphNode::removeChild(GraphNode* child) {
	if (child->parent == this) {
		child->parent = nullptr;
	}
	for (int i = 0; i < children.size(); i++) {
		if (children[i] == child) {
			children.erase(children.begin() + i);
			break;
		}
	}
}

void GraphNode::addComponent(Component* component)
{
	components.push_back(component);
}

void GraphNode::removeComponent(int index)
{
	components.erase(components.begin() + index);
}


template <typename T> Component* GraphNode::getComponent() {
	for(auto &comp : components) {
		Component* result = dynamic_cast<T*>(comp);
		if(result) {
			return result;
		}
	}
	return nullptr;
}

GraphNode* GraphNode::getChild(int index) {
	if (index + 1 <= int(children.size())) {
		return children[index];
	} else {
		return nullptr;
	}
}

Mesh* GraphNode::getMesh() {
	return mesh;
}

GraphNode::~GraphNode() {
	if (mesh != nullptr) {
		delete mesh;
	}
	for (Component* component : components )
	{
		delete component;
	}
	for (auto &child : children) {
		//delete child;
	}
	
}
