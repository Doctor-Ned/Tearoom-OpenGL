#include "Scene/GraphNode.h"
#include "Mesh/Mesh.h"
#include <algorithm>
#include <iostream>
#include <glm/gtx/matrix_decompose.inl>
#include "OctreeNode.h"
#include "Components/Collider.h"
#include "Serialization/DataSerializer.h"
#include "Serialization/Serializer.h"
#include "Scene/Scenes/EditorScene.h"
#include "Components/LightComponents/DirLightComp.h"
#include "Components/LightComponents/SpotLightComp.h"
#include "Components/LightComponents/PointLightComp.h"

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
		GLenum rm = mesh->getRenderMode();
		if (tempRenderMode != GL_NONE) {
			mesh->setRenderMode(tempRenderMode);
		}
		mesh->drawSelf(shader, worldTransform.getMatrix());
		if (tempRenderMode != GL_NONE) {
			mesh->setRenderMode(rm);
		}
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
	if (mesh || getComponent<Renderable>() != nullptr) {
		OctreeNode::toInsert2.insert(this);
	}
	for (Component* component : components) {
		if (component->isComponentActive()) {
			//component->update(timeDiff);
			component->updateComponent(timeDiff);
		}
	}
	for (auto &child : children) {
		child->update(timeDiff);
	}
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

void GraphNode::swapChildren(int index1, int index2) {
	if(index1 == index2 || index1 < 0 || index2 < 0 || index1 + 1 > children.size() || index2 +1 > children.size()) {
		return;
	}
	std::swap(children[index1], children[index2]);
}

void GraphNode::setChildFirst(GraphNode* node) {
	auto it = std::find(children.begin(), children.end(), node);
	if(it == children.end()) {
		return;
	}
	children.erase(it);
	children.insert(children.begin(), node);
}

void GraphNode::setChildLast(GraphNode* node) {
	auto it = std::find(children.begin(), children.end(), node);
	if(it == children.end()) {
		return;
	}
	children.erase(it);
	children.push_back(node);
}

int GraphNode::getChildIndex(GraphNode* child) {
	auto it = std::find(children.begin(), children.end(), child);
	if(it == children.end()) {
		return -1;
	}
	return std::distance(children.begin(), it);
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

void GraphNode::move(glm::vec3 direction) {
	localTransform.translate(direction);
}

void GraphNode::moveRelative(float forward, float right, float up) {
	moveForward(forward);
	moveRight(right);
	moveUp(up);
}

void GraphNode::moveForward(float distance, bool allowVertical) {
	if (distance == 0.0f) return;
	moveRelative(glm::vec3(0.0f, 0.0f, -distance), allowVertical);
}

void GraphNode::moveBackward(float distance, bool allowVertical) {
	moveForward(-distance, allowVertical);
}

void GraphNode::moveLeft(float distance) {
	moveRight(-distance);
}

void GraphNode::moveRight(float distance) {
	if (distance == 0.0f) return;
	moveRelative(glm::vec3(distance, 0.0f, 0.0f));
}

void GraphNode::moveUp(float distance) {
	if (distance == 0.0f) return;
	moveRelative(glm::vec3(0.0f, distance, 0.0f));
}

void GraphNode::moveDown(float distance) {
	moveUp(-distance);
}

glm::vec3 GraphNode::getUpVector() {
	return getRelative(glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 GraphNode::getDownVector() {
	return getRelative(glm::vec3(0.0f, -1.0f, 0.0f));
}

glm::vec3 GraphNode::getFrontVector() {
	return getRelative(glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 GraphNode::getBackVector() {
	return getRelative(glm::vec3(0.0f, 0.0f, 1.0f));
}

glm::vec3 GraphNode::getRightVector() {
	return getRelative(glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 GraphNode::getLeftVector() {
	return getRelative(glm::vec3(-1.0f, 0.0f, 0.0f));
}

glm::vec3 GraphNode::getRelative(glm::vec3 base) {
	glm::mat4 world = worldTransform.getUntranslatedMatrix();
	return world * glm::vec4(base, 1.0f);
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
	for (auto &child : children) {
		delete child;
	}
}

bool GraphNode::isOpaque() {
	return mesh != nullptr && mesh->isOpaque();
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
	for (int i = 0; i < components.size(); i++) {
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
	for (int i = 0; i < root["components"].size(); i++) {
		addComponent(dynamic_cast<Component*>(serializer->deserialize(root["components"][i]).object));
	}
	mesh = dynamic_cast<Mesh*>(serializer->deserialize(root["mesh"]).object);
	setParent(dynamic_cast<GraphNode*>(serializer->deserialize(root["parent"]).object));
}

SerializableType GraphNode::getSerializableType() {
	return SGraphNode;
}

void GraphNode::setTempRenderMode(GLenum tempRenderMode) {
	this->tempRenderMode = tempRenderMode;
}

void GraphNode::removeTempRenderMode() {
	this->tempRenderMode = GL_NONE;
}

glm::vec3 GraphNode::getPosition() {
	return worldTransform.getPosition();
}

int GraphNode::getChildrenCount() {
	return children.size();
}

void GraphNode::moveRelative(glm::vec3 direction, bool allowVertical) {
	direction = getRelative(direction);
	if(!allowVertical) {
		direction.y = 0.0f;
	}
	move(direction);
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
	ImGui::Checkbox((name + " - active").c_str(), &active);
	if (active != this->active)setActive(active);
	ImGui::NewLine();
	if (active) {
		static const int BUFF_SIZE = 50;
		static char buff[BUFF_SIZE] = "";
		if (buff[0] == '\0') {
			for (int i = 0; i < std::min(BUFF_SIZE, static_cast<int>(name.length())); i++) {
				buff[i] = name[i];
			}
		}
		ImGui::InputText("Name", buff, IM_ARRAYSIZE(buff));
		std::string newName(Global::trim(buff));
		if (newName.length() > 0 && newName != name && ImGui::Button("Apply name")) {
			setName(newName);
			buff[0] = '\0';
		}
		ImGui::Text("_____________________");
		localTransform.drawGui();
		ImGui::Text("_____________________");
		EditorScene *editor = GameManager::getInstance()->getEditorScene();
		if (mesh != nullptr) {
			if (editor != nullptr && editor->confirmationDialogCallback == nullptr && ImGui::Button("Delete mesh")) {
				editor->confirmationDialogCallback = [this, editor]() {
					//delete mesh;
					mesh = nullptr;
					editor->editedScene->updateRenderable(this);
				};
			}
			mesh->drawGui();
			ImGui::Text(("Shader type: " + ShaderTypeNames[static_cast<int>(mesh->getShaderType())]).c_str());
			if (editor != nullptr && editor->shaderTypeSelectionCallback == nullptr) {
				ImGui::SameLine();
				if (ImGui::Button("Change...")) {
					editor->shaderTypeSelectionCallback = [node = this, editor = editor, mesh = mesh](ShaderType type) {
						mesh->setShaderType(type);
						editor->editedScene->updateRenderable(node);
					};
				}
			}
		} else {
			if (editor != nullptr && editor->meshSelectionCallback == nullptr && ImGui::Button("Add mesh...")) {
				editor->meshSelectionCallback = [this, editor](SerializableType type) {
					if (!editor->typeCreationExists(type)) {
						editor->addTypeCreation(type, [this, editor](void* mesh) {
							this->mesh = reinterpret_cast<Mesh*>(mesh);
							editor->editedScene->updateRenderable(this, true);
						});
					}
				};
			}
		}
		if (editor != nullptr && editor->componentSelectionCallback == nullptr && ImGui::Button("Add component...")) {
			editor->componentSelectionCallback = [this,editor](SerializableType type) {
				if(!editor->typeCreationExists(type)) {
					editor->addTypeCreation(type, [this, editor](void *component) {
						this->addComponent(reinterpret_cast<Component*>(component));
						Renderable *rend = dynamic_cast<Renderable*>(reinterpret_cast<Component*>(component));
						if(rend != nullptr) {
							editor->editedScene->addToRenderMap(rend);
						}
					}, this);
				}
			};
		}
		if (!components.empty() && ImGui::TreeNode("Components")) {
			int counter = 0;
			for (auto &comp : components) {
				if (ImGui::TreeNode((comp->getName() + "(" + SerializableTypeNames[comp->getSerializableType()] + ")").c_str())) {
					if(editor != nullptr && editor->confirmationDialogCallback == nullptr) {
						ImGui::SameLine();
						if(ImGui::Button("Delete")) {
							editor->confirmationDialogCallback = [this,comp,editor]() {
								editor->editedScene->removeComponent(this, comp);
							};
						}
					}
					comp->drawGui();
					ImGui::TreePop();
					counter++;
				}
			}
			ImGui::TreePop();
		}
	}
}
