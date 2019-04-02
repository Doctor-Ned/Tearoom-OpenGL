#include "GraphNode.h"
#include "Mesh/Mesh.h"
#include <algorithm>
#include <iostream>
#include <glm/gtx/matrix_decompose.inl>

GraphNode::GraphNode(Mesh* mesh, GraphNode* parent) : parent(parent), mesh(mesh), dirty(true), localTransform(Transform(dirty)), worldTransform(Transform(dirty)) {
	this->name = "Node";
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
	if (!active) {
		return;
	}
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
			mesh->draw(worldTransform.Matrix());
		} else {
			mesh->draw(shader, worldTransform.Matrix());
		}
	}

	for (Component* component : components) {
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
	if (!active) {
		return;
	}
	for (Component* component : components) {
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

void GraphNode::addComponent(Component* component) {
	for (auto comp : components) {
		if (comp == component) {
			return;
		}
	}
	components.push_back(component);
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
	for (Component* component : components) {
		delete component;
	}
	for (auto &child : children) {
		//delete child;
	}

}

bool GraphNode::isActive() {
	return active;
}

void GraphNode::setActive(bool active) {
	this->active = active;
}

const char* GraphNode::getName() {
	return name;
}

void GraphNode::setName(const char* name) {
	this->name = name;
}

void GraphNode::renderGui() {
	std::string title(name);
	title.append(" - active");
	ImGui::Checkbox(title.c_str(), &active);
	ImGui::NewLine();
	if (active) {
		//glm::vec3 scale;
		//glm::quat rotation;
		//glm::vec3 translation;
		//glm::vec3 skew;
		//glm::vec4 perspective;
		//decompose(localTransform.Matrix(), scale, rotation, translation, skew, perspective);
		//rotation = conjugate(rotation);
		//glm::vec3 euler = eulerAngles(rotation);
		//ImGui::InputFloat3("Position", &translation[0]);
		//ImGui::NewLine();
		//ImGui::SliderAngle("Rotation", &euler[0]);
		//ImGui::NewLine();
		//ImGui::InputFloat3("Scale", &scale[0]);
		//ImGui::NewLine();
		//glm::mat4 matrix = translate(glm::mat4(1.0f), translation);
		//matrix = glm::scale(matrix, scale);
		//matrix = mat4_cast(glm::quat(euler)) * matrix;
		//localTransform.SetMatrix(matrix);

		ImGui::InputFloat3("Position (fixed)", reinterpret_cast<float*>(&localTransform.transform[3]));
		ImGui::NewLine();
		ImGui::SliderFloat3("Position (slider)", reinterpret_cast<float*>(&localTransform.transform[3]), -10.0f, 10.0f);
		ImGui::NewLine();

		if(ImGui::Button("RotX-")) {
			localTransform.Rotate(-15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (ImGui::Button("RotY-")) {
			localTransform.Rotate(-15.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (ImGui::Button("RotZ-")) {
			localTransform.Rotate(-15.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		ImGui::NewLine();
		if (ImGui::Button("RotX+")) {
			localTransform.Rotate(15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (ImGui::Button("RotY+")) {
			localTransform.Rotate(15.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (ImGui::Button("RotZ+")) {
			localTransform.Rotate(15.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		ImGui::NewLine();
	}
}
