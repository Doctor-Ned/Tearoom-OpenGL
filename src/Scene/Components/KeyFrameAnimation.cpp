#include "KeyFrameAnimation.h"
#include "Scene/GraphNode.h"

KeyFrameAnimation::KeyFrameAnimation(GraphNode* gameObject, std::string&& name) : Animation(gameObject, std::move(name)) {
	KeyFrameAnimation::takeObjectsToAnimate(gameObject);
}

void KeyFrameAnimation::setFrame(float time) {
	if (time < 0.0f) {
		currentTime = 0.0f;
	} else if (time > endTime) {
		currentTime = endTime;
	} else {
		currentTime = time;
	}
	interpolateObjects();
}

void KeyFrameAnimation::update(float msec) {
	if (isPlaying) {
		interpolateObjects();
		currentTime += msec * speed;
	}

	if (currentTime >= endTime) {
		if (!looped) {
			isPlaying = false;
		}
		currentTime = 0.0f;
	}
}

SerializableType KeyFrameAnimation::getSerializableType() {
	return SKeyFrameAnimation;
}

void KeyFrameAnimation::deserialize(Json::Value& root, Serializer* serializer) {
	Animation::deserialize(root, serializer);
}

void KeyFrameAnimation::renderGui() {
	Animation::renderGui();
	if (isComponentActive()) {
		if (!isPlaying && ImGui::Button("Go back to start")) {
			setFrame(0.0f);
		}
		ImGui::Text("If anim doesn't work, try reload.");
		if (ImGui::Button("Reload")) {
			takeObjectsToAnimate(gameObject);
		}
	}
}

void KeyFrameAnimation::takeObjectsToAnimate(GraphNode* objectToAnimate) {
	if (objectToAnimate) {
		objectsToAnimate.push_back(objectToAnimate);
		for (GraphNode* node : objectToAnimate->getChildren()) {
			takeObjectsToAnimate(node);
		}
	}
}

void KeyFrameAnimation::interpolateObjects() {
	for (GraphNode* gameObject : objectsToAnimate) {
		auto animForGameObject = animationData->keyFramesForObjects.find(gameObject->getName());
		if (animForGameObject != animationData->keyFramesForObjects.end()) {
			interpolateValues(currentTime, gameObject, Animated::TRANSLATION, animForGameObject->second.translation);
			interpolateValues(currentTime, gameObject, Animated::SCALE, animForGameObject->second.scale);
			interpolateValues(currentTime, gameObject, Animated::ROTATION, animForGameObject->second.rotation);
		}
	}
}
