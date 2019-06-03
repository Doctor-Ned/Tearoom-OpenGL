#include "AnimationController.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"
#include "Serialization/Serializer.h"
#include "Scene/Scenes/EditorScene.h"
#include "Scene/SoundSystem.h"

void AnimationController::update(float msec) {
	if(anim != nullptr) {
		anim->setComponentActive(false);
	}
}

void AnimationController::renderGui() {
	Component::renderGui();
	if(active ) {
		ImGui::DragInt("Door ID", &doorID, 1, 0, 100);
		ImGui::Text(("Animation acquired from: " + (anim == nullptr ? "None" : anim->getGameObject()->getName())).c_str());
		EditorScene *editor = gameManager->getEditorScene();
		if(editor && editor->nodeSelectionCallback == nullptr) {
			ImGui::SameLine();
			if(ImGui::Button("CHOOSE")) {
				editor->nodeSelectionCallback = [this](GraphNode *node) {
					anim = node->getComponent<Animation>();
				};
			}
		}
		if(ImGui::Button("ACQUIRE OWN")) {
			anim = gameObject->getComponent<Animation>();
		}
	}
}

SerializableType AnimationController::getSerializableType() {
	return SAnimationController;
}

Json::Value AnimationController::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["doorID"] = doorID;
	root["anim"] = serializer->serialize(anim);
	return root;
}

void AnimationController::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	doorID = root["doorID"].asInt();
	anim = dynamic_cast<Animation*>(serializer->deserialize(root["anim"]).object);
}

AnimationController::AnimationController(GraphNode* _gameObject) : Component(_gameObject, "Animation Controller") {}

AnimationController::AnimationController(GraphNode* _gameObject, int doorID, Animation* anim) : Component(_gameObject, "Animation Controller"), doorID(doorID), anim(anim) {}

int AnimationController::getDoorID() {
	return doorID;
}

void AnimationController::open() {
	setComponentActive(false);
	SoundSystem::getSound("doorOpeningSound")->setDefaultVolume(0.15);
	SoundSystem::getEngine()->play2D(SoundSystem::getSound("doorOpeningSound"));
	anim->setComponentActive(true);
	anim->play();
}

