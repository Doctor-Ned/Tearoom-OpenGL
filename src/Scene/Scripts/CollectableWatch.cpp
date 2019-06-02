#include "CollectableWatch.h"
#include "Scene/Components/SunController.h"
#include "Serialization/Serializer.h"
#include "Scene/Scenes/EditorScene.h"
#include "CollectableObject.h"

CollectableWatch::CollectableWatch(GraphNode* _gameObject) : Component(_gameObject, "Collectable Watch") {
	collectable = new CollectableObject(_gameObject, Letter, "res/textures/Icons/letterIcon.png", glm::vec2(995.0f, 530.0f), glm::vec2(60.0f, 60.0f),
										"Yoshiro's letter about the watch", "res/textures/Letter/clockLetterPreview.PNG", glm::vec2(1200.0f, 430.0f), glm::vec2(300.0f, 500.0f));
	_gameObject->addComponent(collectable);
}

void CollectableWatch::update(float msec) {}

void CollectableWatch::pickup() {
	if (sunController) {
		sunController->setComponentActive(true);
		sunController->setClockVisibility(true);
	}
	this->setComponentActive(false);
}

SerializableType CollectableWatch::getSerializableType() {
	return SCollectableWatch;
}

Json::Value CollectableWatch::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["sunController"] = serializer->serialize(sunController);
	root["collectable"] = serializer->serialize(collectable);
	return root;
}

void CollectableWatch::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	sunController = dynamic_cast<SunController*>(serializer->deserialize(root["sunController"]).object);
	collectable = dynamic_cast<CollectableObject*>(serializer->deserialize(root["collectable"]).object);
}

void CollectableWatch::setSunController(SunController* sunController) {
	this->sunController = sunController;
}

SunController* CollectableWatch::getSunController() {
	return sunController;
}

CollectableObject* CollectableWatch::getCollectable() {
	return collectable;
}

void CollectableWatch::renderGui() {
	Component::renderGui();
	if(isComponentActive()) {
		ImGui::Text(("Sun controller acquired from: " + (sunController == nullptr ? "None" : sunController->getGameObject()->getName())).c_str());
		EditorScene *editor = gameManager->getEditorScene();
		if(editor && editor->nodeSelectionCallback == nullptr && ImGui::Button("CHOOSE SUN CONTROLLER")) {
			editor->nodeSelectionCallback = [this](GraphNode *node) {
				setSunController(node->getComponent<SunController>());
			};
		}
		if(ImGui::Button("SET NULL")) {
			setSunController(nullptr);
		}
	}
}
