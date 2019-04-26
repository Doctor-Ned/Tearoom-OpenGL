#include "Scene/GraphNode.h"
#include "Mesh/Mesh.h"
#include <algorithm>
#include <iostream>
#include <glm/gtx/matrix_decompose.inl>
#include "OctreeNode.h"
#include "Components/Collider.h"
#include "Serialization/DataSerializer.h"
#include "Serialization/Serializer.h"

GraphNode::GraphNode(Mesh* mesh, GraphNode* parent) : parent(parent), mesh(mesh), dirty(true), localTransform(ComposedTransform(dirty)), worldTransform(Transform(dirty)) {
	this->name = "Node";
	if (parent != nullptr) {
		parent->addChild(this);
	}
}

void GraphNode::updateDrawData() {
	if (!active) {
		return;
	}
	if (parent != nullptr) {
		dirty |= parent->dirty;
		if (dirty) {
			updateWorld();
		}
	} else if (dirty) {
		updateWorld();
	}

	//if (mesh && !(ignoreLights && mesh->getShaderType() != STLight)) {
	//	if (shader == nullptr) {
	//		mesh->draw(worldTransform.Matrix());
	//	} else {
	//		mesh->draw(shader, worldTransform.Matrix());
	//	}
	//}

	for (Renderable* renderable : renderableComponents) {
		Component *comp = dynamic_cast<Component*>(renderable);
		if (renderable->isActive() && comp->isComponentActive()) {
			renderable->updateDrawData();
		}
	}

	for (auto &child : children) {
		child->updateDrawData();
	}

	dirty = false;
}

void GraphNode::drawSelf(Shader *shader) {
	if (mesh != nullptr) {
		mesh->drawSelf(shader, worldTransform.getMatrix());
	}
}

ShaderType GraphNode::getShaderType() {
	if (mesh == nullptr) {
		return STNone;
	}
	return mesh->getShaderType();
}

void GraphNode::update(double timeDiff) {
	if (!active) {
		return;
	}
	if (mesh || !getComponents<Renderable>().empty()) {
		OctreeNode::toInsert2.insert(this);
	}
	for (Component* component : components) {
		if (component->isComponentActive()) {
			component->update(timeDiff);
		}
	}
	for (auto &child : children) {
		child->update(timeDiff);
	}

	hitByRay = false;
}

GraphNode* GraphNode::getParent() const {
	return parent;
}

void GraphNode::setParent(GraphNode* parent) {
	if (this->parent != nullptr) {
		this->parent->removeChild(this);
	}
	if (parent != nullptr) {
		parent->addChild(this);
	}
	dirty = true;
}

void GraphNode::addChild(GraphNode* child) {
	if (child == nullptr) {
		return;
	}
	child->parent = this;
	if (std::find(children.begin(), children.end(), child) == children.end()) {
		children.push_back(child);
	}
}

void GraphNode::removeChild(GraphNode* child) {
	if (child == nullptr) {
		return;
	}
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

void GraphNode::addComponent(Component* component) {
	for (auto comp : components) {
		if (comp == component) {
			return;
		}
	}
	components.push_back(component);
	Renderable *r = dynamic_cast<Renderable*>(component);
	if (r) {
		renderableComponents.push_back(r);
	}
}

std::vector<Component*> GraphNode::getComponents() {
	return getComponents<Component>();
}

GraphNode* GraphNode::getChild(int index) {
	if (index + 1 <= int(children.size())) {
		return children[index];
	} else {
		return nullptr;
	}
}

float GraphNode::getOpacity() {
	return mesh == nullptr ? 0.0f : mesh->getOpacity();
}

void GraphNode::setOpacity(float opacity) {
	if (mesh != nullptr) {
		mesh->setOpacity(opacity);
	}
}

Mesh* GraphNode::getMesh() {
	return mesh;
}

GraphNode::~GraphNode() {
	if (mesh != nullptr) {
		delete mesh;
	}
	for (Component* component : components) {
		delete component;
	}
	renderableComponents.clear();
	//for (auto &child : children) {
		//delete child;
	//}
}

bool GraphNode::isOpaque() {
	return mesh == nullptr || mesh->isOpaque();
}

bool GraphNode::isActive() {
	return active;
}

void GraphNode::setActive(bool active) {
	this->active = active;
}

std::string GraphNode::getName() const {
	return name;
}

void GraphNode::setName(std::string name) {
	this->name = name;
}

void GraphNode::beingHitByRay() {
	hitByRay = true;
}

bool GraphNode::getHitByRay() {
	return  hitByRay;
}

std::vector<GraphNode*> GraphNode::getChildren() const {
	return children;
}

std::vector<Renderable*> GraphNode::getRenderableComponents() const {
	return renderableComponents;
}

Json::Value GraphNode::serialize(Serializer* serializer) {
	Json::Value root;
	root["name"] = name;
	root["active"] = active;
	root["localTransformData"] = DataSerializer::serializeTransformData(localTransform.getData());
	root["localLastTransformData"] = DataSerializer::serializeTransformData(localTransform.getLastData());
	for (int i = 0; i < children.size(); i++) {
		root["children"][i] = serializer->serialize(children[i]);
	}
	for(int i=0;i<components.size();i++) {
		root["components"][i] = serializer->serialize(components[i]);
	}
	root["parent"] = serializer->serialize(parent);
	root["mesh"] = serializer->serialize(mesh);
	return root;
}

void GraphNode::deserialize(Json::Value& root, Serializer* serializer) {
	name = root.get("name", name).asString();
	active = root.get("active", active).asBool();
	localTransform.setData(DataSerializer::deserializeTransformData(root["localTransformData"]));
	localTransform.setLastData(DataSerializer::deserializeTransformData(root["localLastTransformData"]));
	Json::Value children = root["children"];
	int size = children.size();
	for (int i = 0; i < size; i++) {
		addChild(dynamic_cast<GraphNode*>(serializer->deserialize(children[i]).object));
	}
	for(int i=0;i<root["components"].size();i++) {
		addComponent(dynamic_cast<Component*>(serializer->deserialize(root["components"][i]).object));
	}
	mesh = dynamic_cast<Mesh*>(serializer->deserialize(root["mesh"]).object);
	setParent(dynamic_cast<GraphNode*>(serializer->deserialize(root["parent"]).object));
}

SerializableType GraphNode::getSerializableType() {
	return SGraphNode;
}

void GraphNode::updateWorld() {
	if (dirty) {
		localTransform.updateLast();
		if (parent != nullptr) {
			worldTransform.setMatrix(parent->worldTransform.getMatrix() * localTransform.getMatrix());
		} else {
			worldTransform.setMatrix(localTransform.getMatrix());
		}
	}
	for (auto &comp : components) {
		if (comp->isComponentActive()) {
			comp->updateWorld();
		}
	}
}

void GraphNode::renderGui() {
	bool active = this->active;
	ImGui::Checkbox(name.c_str(), &active);
	if (active != this->active)setActive(active);
	ImGui::NewLine();
	if (active) {
		localTransform.drawGui();
		for (auto &comp : components) {
			comp->renderGui();
			ImGui::NewLine();
		}
	}
}
