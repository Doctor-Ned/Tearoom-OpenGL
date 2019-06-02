#include "AnimTimeSaver.h"
#include "Scene/GraphNode.h"
#include "Scene/Scenes/EditorScene.h"
#include "Serialization/Serializer.h"
#include "SunTimeActivator.h"

void AnimTimeSaver::renderGui() {
	Component::renderGui();
	if (active) {
		ImGui::Text(("Saved anim time: " + std::to_string(targetTime)).c_str());
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
		ImGui::Text(("KeyFrameAnimation acquired from: " + (animation == nullptr ? "None" : animation->getGameObject()->getName())).c_str());
		if (editor && editor->nodeSelectionCallback == nullptr) {
			ImGui::SameLine();
			if (ImGui::Button("CHOOSE ANIMATION")) {
				editor->nodeSelectionCallback = [this](GraphNode *node) {
					this->setAnimation(node->getComponent<KeyFrameAnimation>());
				};
			}
		}
		if (ImGui::Button("RESET")) {
			reset();
		}
		if (ImGui::Button("ACQUIRE OWN")) {
			Sun *sun = gameObject->getComponent<Sun>();
			KeyFrameAnimation *anim = gameObject->getComponent<KeyFrameAnimation>();
			if (sun != nullptr) {
				setSun(sun);
			}
			if (anim != nullptr) {
				setAnimation(anim);
			}
		}
		if (ImGui::Button("SET NULLS")) {
			setSun(nullptr);
			setAnimation(nullptr);
		}
	}
}

void AnimTimeSaver::reset() {
	targetTime = -1;
	startedPlaying = false;
	retargetAllowed = false;
}

Sun* AnimTimeSaver::getSun() {
	return sun;
}

KeyFrameAnimation* AnimTimeSaver::getAnimation() {
	return animation;
}

void AnimTimeSaver::setSun(Sun* sun) {
	this->sun = sun;
	reset();
}

void AnimTimeSaver::setAnimation(KeyFrameAnimation* animation) {
	this->animation = animation;
	reset();
}

SerializableType AnimTimeSaver::getSerializableType() {
	return SAnimTimeSaver;
}

Json::Value AnimTimeSaver::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["sun"] = serializer->serialize(sun);
	root["animation"] = serializer->serialize(animation);
	root["targetTime"] = targetTime;
	root["startedPlaying"] = startedPlaying;
	root["retargetAllowed"] = retargetAllowed;
	return root;
}

void AnimTimeSaver::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	sun = dynamic_cast<Sun*>(serializer->deserialize(root["sun"]).object);
	animation = dynamic_cast<KeyFrameAnimation*>(serializer->deserialize(root["animation"]).object);
	targetTime = root["targetTime"].asInt();
	startedPlaying = root["startedPlaying"].asBool();
	retargetAllowed = root["retargetAllowed"].asBool();
}

AnimTimeSaver::AnimTimeSaver(GraphNode* _gameObject) : Component(_gameObject, "AnimTimeSaver") {
	reset();
}

void AnimTimeSaver::update(float msec) {
	if (sun != nullptr && animation != nullptr) {
		if (animation->getIsPlaying()) {
			if (!startedPlaying || retargetAllowed) {
				startedPlaying = true;
				targetTime = round(sun->getTime());
			}
		} else {
			if (startedPlaying) {
				float time = sun->getTime();
				float curr = animation->getCurrentTime();
				float end = animation->getEndTime();
				SunTimeActivator *activator = gameObject->getComponent<SunTimeActivator>();
				if (time >= targetTime) {
					animation->setComponentActive(false);
					retargetAllowed = false;
					if(curr != end) {
						animation->setFrame(end);
					}
				} else {
					if(activator == nullptr || activator->isTimeCorrect()) animation->setComponentActive(true);
					retargetAllowed = true;
					if(time < targetTime - 1) {
						animation->setFrame(0.0f);
					} else {
						animation->setFrame(end * (time + 1.0f - targetTime));
					}
				}
			}
		}
	}
}
