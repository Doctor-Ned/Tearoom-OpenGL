#include "SunTimeActivator.h"
#include "Scene/GraphNode.h"
#include "Scene/Scenes/EditorScene.h"
#include "Serialization/Serializer.h"

void SunTimeActivator::renderGui() {
	Component::renderGui();
	if (active) {
		ImGui::DragInt("Target time", &targetTime, 1, 7, 17);
		ImGui::Text(("Sun acquired from: " + (sun == nullptr ? "None" : sun->getGameObject()->getName())).c_str());
		EditorScene *editor = gameManager->getEditorScene();
		if (editor && editor->nodeSelectionCallback == nullptr) {
			ImGui::SameLine();
			if (ImGui::Button("CHOOSE SUN")) {
				editor->nodeSelectionCallback = [this](GraphNode *node) {
					this->setSun(node->getComponent<Sun>());
				};
			}
		}
		static std::vector<Component*> toRemove;
		for (auto &comp : components) {
			ImGui::PushID(comp);
			ImGui::Text((comp->getName() + " acquired from " + comp->getGameObject()->getName()).c_str());
			ImGui::SameLine();
			if (ImGui::Button("Remove")) {
				toRemove.push_back(comp);
			}
			ImGui::PopID();
		}
		for (auto &comp : toRemove) {
			removeActivatableComponent(comp);
		}
		toRemove.clear();

		if (ImGui::Button("ACQUIRE OWN")) {
			clearActivatableComponents();
			for (auto &comp : gameObject->getComponents()) {
				addActivatableComponent(comp);
			}
		}
		if (ImGui::Button("SET NULLS")) {
			setSun(nullptr);
			clearActivatableComponents();
		}
	}
}

Sun* SunTimeActivator::getSun() {
	return sun;
}

void SunTimeActivator::setSun(Sun* sun) {
	this->sun = sun;
}

std::vector<Component*> SunTimeActivator::getActivatableComponents() {
	return components;
}

void SunTimeActivator::addActivatableComponent(Component* component) {
	if (component == this || component == nullptr) {
		return;
	}
	for (auto &comp : components) {
		if (comp == component) {
			return;
		}
	}
	components.push_back(component);
}

void SunTimeActivator::removeActivatableComponent(Component* component) {
	for (auto i = components.begin(); i != components.end();) {
		if (*i == component) {
			components.erase(i);
			break;
		}
		++i;
	}
}

void SunTimeActivator::clearActivatableComponents() {
	components.clear();
}

bool SunTimeActivator::hasComponent(Component* comp) {
	for(auto &c : components) {
		if (c==comp) {
			return true;
		}
	}
	return false;
}

bool SunTimeActivator::isTimeCorrect() {
	return correct;
}

SerializableType SunTimeActivator::getSerializableType() {
	return SSunTimeActivator;
}

SunTimeActivator::SunTimeActivator(GraphNode* _gameObject) : Component(_gameObject) {}

void SunTimeActivator::update(float msec) {
	if (sun != nullptr) {
		correct = abs(targetTime - sun->getTime()) < 0.1f;
		for (auto &comp : components) {
			comp->setComponentActive(correct);
		}
	}
}

Json::Value SunTimeActivator::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["sun"] = serializer->serialize(sun);
	for (int i = 0; i < components.size(); i++) {
		root["components"][i] = serializer->serialize(components[i]);
	}
	root["targetTime"] = targetTime;
	root["correct"] = correct;
	return root;
}

void SunTimeActivator::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	sun = dynamic_cast<Sun*>(serializer->deserialize(root["sun"]).object);
	for (int i = 0; i < root["components"].size(); i++) {
		addActivatableComponent(dynamic_cast<Component*>(serializer->deserialize(root["components"][i]).object));
	}
	targetTime = root["targetTime"].asInt();
	correct = root.get("correct", correct).asBool();
}