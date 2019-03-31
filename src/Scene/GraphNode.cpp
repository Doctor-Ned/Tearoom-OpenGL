#include "GraphNode.h"
#include "Mesh/Mesh.h"
#include <algorithm>

GraphNode::GraphNode(Mesh* mesh, GraphNode* parent) : //local(glm::mat4(1.0f)), 
parent(parent), mesh(mesh), dirty(true), scale(1.0f), localTransform(Transform(dirty)), worldTransform(Transform(dirty)) {
	if (parent != nullptr) {
		parent->addChild(this);
	}
}

void GraphNode::draw() {
	if (parent != nullptr) {
		dirty |= parent->dirty;
		if (dirty) {
			//world = parent->getWorld() * local;
			worldTransform.SetMatrix(parent->worldTransform.Matrix() * localTransform.Matrix());
		}
	} else if (dirty) {
		//world = local;
		worldTransform.SetMatrix(localTransform.Matrix());
	}

	if (mesh) {
		//mesh->draw(world, scale);
		mesh->draw(worldTransform.Matrix(), scale);
	}

	for (int i = 0; i < children.size(); i++) {
		children[i]->draw();
	}
	if (dirty) {
		dirty = false;
	}
}

void GraphNode::draw(GraphNode* excluded) {
	if (parent != nullptr) {
		dirty |= parent->dirty;
		if (dirty) {
			//world = parent->getWorld() * local;
			worldTransform.SetMatrix(parent->worldTransform.Matrix() * localTransform.Matrix());
		}
	} else if (dirty) {
		//world = local;
		worldTransform.SetMatrix(localTransform.Matrix());
	}

	if (mesh) {
		//mesh->draw(world, scale);
		mesh->draw(worldTransform.Matrix(), scale);
	}

	for (int i = 0; i < children.size(); i++) {
		if (children[i] != excluded) {
			children[i]->draw(excluded);
		}
	}
	if (dirty) {
		dirty = false;
	}
}

void GraphNode::draw(std::vector<GraphNode*> excluded) {
	if (parent != nullptr) {
		dirty |= parent->dirty;
		if (dirty) {
			//world = parent->getWorld() * local;
			worldTransform.SetMatrix(parent->worldTransform.Matrix() * localTransform.Matrix());
		}
	} else if (dirty) {
		//world = local;
		worldTransform.SetMatrix(localTransform.Matrix());
	}

	if (mesh) {
		//mesh->draw(world, scale);
		mesh->draw(worldTransform.Matrix(), scale);
	}

	for (int i = 0; i < children.size(); i++) {
		bool exclude = false;
		for (int j = 0; j < excluded.size(); j++) {
			if (children[i] == excluded[j]) {
				exclude = true;
				break;
			}
		}
		if (!exclude) {
			children[i]->draw(excluded);
		}
	}
	if (dirty) {
		dirty = false;
	}
}

void GraphNode::draw(Shader* shader) {
	if (parent != nullptr) {
		dirty |= parent->dirty;
		if (dirty) {
			//world = parent->getWorld() * local;
			worldTransform.SetMatrix(localTransform.Matrix());
		}
	} else if (dirty) {
		//world = local;
		mesh->draw(worldTransform.Matrix(), scale);
	}

	if (mesh) {
		//mesh->draw(shader, world, scale);
		mesh->draw(shader, worldTransform.Matrix(), scale);
	}

	for (int i = 0; i < children.size(); i++) {
		children[i]->draw(shader);
	}
	if (dirty) {
		dirty = false;
	}
}

void GraphNode::draw(Shader* shader, GraphNode* excluded) {
	if (parent != nullptr) {
		dirty |= parent->dirty;
		if (dirty) {
			//world = parent->getWorld() * local;
			worldTransform.SetMatrix(parent->worldTransform.Matrix() * localTransform.Matrix());
		}
	} else if (dirty) {
		//world = local;
		worldTransform.SetMatrix(localTransform.Matrix());
	}

	if (mesh) {
		//mesh->draw(shader, world, scale);
		mesh->draw(shader, worldTransform.Matrix(), scale);
	}

	for (int i = 0; i < children.size(); i++) {
		if (children[i] != excluded) {
			children[i]->draw(shader, excluded);
		}
	}
	if (dirty) {
		dirty = false;
	}
}

void GraphNode::draw(Shader* shader, std::vector<GraphNode*> excluded) {
	if (parent != nullptr) {
		dirty |= parent->dirty;
		if (dirty) {
			//world = parent->getWorld() * local;
			worldTransform.SetMatrix(parent->worldTransform.Matrix() * localTransform.Matrix());
		}
	} else if (dirty) {
		//world = local;
		worldTransform.SetMatrix(localTransform.Matrix());
	}

	if (mesh) {
		//mesh->draw(shader, world, scale);
		mesh->draw(shader, worldTransform.Matrix(), scale);
	}

	for (int i = 0; i < children.size(); i++) {
		bool exclude = false;
		for (int j = 0; j < excluded.size(); j++) {
			if (children[i] == excluded[j]) {
				exclude = true;
				break;
			}
		}
		if (!exclude) {
			children[i]->draw(shader, excluded);
		}
	}
	if (dirty) {
		dirty = false;
	}
}

void GraphNode::update(double timeDiff) {
	for (int i = 0; i < children.size(); i++) {
		children[i]->update(timeDiff);
	}
}

//glm::mat4 GraphNode::getWorld() const {
//	return world;
//}

GraphNode* GraphNode::getParent() const {
	return parent;
}

void GraphNode::setParent(GraphNode* parent, bool preserveWorldPosition) {
	if(preserveWorldPosition) {
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

//glm::mat4 GraphNode::getLocal() const {
//	return local;
//}

//void GraphNode::setLocal(glm::mat4 local) {
//	this->local = local;
//	dirty = true;
//}

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

GraphNode* GraphNode::getChild(int index)
{
	if (index + 1 <= int(children.size()))
	{
		return children[index];
	}
	else
	{
		return nullptr;
	}
}

Mesh* GraphNode::getMesh() {
	return mesh;
}

GraphNode::~GraphNode() {
	for(auto &child : children) {
		delete child;
	}
}
