#include "KeyFrameAnimation.h"
#include "Scene/GraphNode.h"

using anim::Animated;
using anim::animMap;
using anim::ObjectAnimation;

KeyFrameAnimation::KeyFrameAnimation(GraphNode* gameObject, std::string&& name) : Animation(gameObject, std::move(name)) {
	takeObjectsToAnimate(gameObject);
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
	takeObjectsToAnimate(gameObject);
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

void KeyFrameAnimation::interpolateValues(float currentTime, GraphNode* animatedObject, Animated type, std::map<float, glm::vec3>& mapToInterpolate) {
	if (mapToInterpolate.size() < 2)
		return;
	keyFramePair itPair = getProperIterators(currentTime, mapToInterpolate);
	anim::animMap::iterator leftKeyFrame = itPair.first;
	anim::animMap::iterator rightKeyFrame = itPair.second;

	if (rightKeyFrame == mapToInterpolate.end())
		return;

	float time = rightKeyFrame->first - leftKeyFrame->first;
	currentTime = currentTime - leftKeyFrame->first;
	glm::vec3 mix = glm::mix(leftKeyFrame->second, rightKeyFrame->second, currentTime / time);

	if (type == Animated::TRANSLATION) {
		animatedObject->localTransform.setPosition(mix);
	} else if (type == Animated::SCALE) {
		animatedObject->localTransform.setScale(mix);
	} else if (type == Animated::ROTATION) {
		animatedObject->localTransform.setRotationDegrees(mix);
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
		auto animForGameObject = objectAnimations.find(gameObject->getName());
		if (animForGameObject != objectAnimations.end()) {
			interpolateValues(currentTime, gameObject, Animated::TRANSLATION, animForGameObject->second.translation);
			interpolateValues(currentTime, gameObject, Animated::SCALE, animForGameObject->second.scale);
			interpolateValues(currentTime, gameObject, Animated::ROTATION, animForGameObject->second.rotation);
		}
	}
}
