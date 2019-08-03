#include "SkeletalAnimation.h"
#include "Scene/GraphNode.h"
#include "Mesh/AnimatedModel.h"

void SkeletalAnimation::interpolate(Bone* animatedObject) {
	auto animForGameObject = animationData->keyFramesForObjects.find(animatedObject->name);
	if (animForGameObject != animationData->keyFramesForObjects.end()) {
		interpolateValues(currentTime, animatedObject, TRANSLATION, animForGameObject->second.translation);
		interpolateValues(currentTime, animatedObject, SCALE, animForGameObject->second.scale);
		interpolateValues(currentTime, animatedObject, ROTATION,animForGameObject->second.rotation);
	}

	for (auto& child : animatedObject->children) {
		interpolate(child);
	}
}

void SkeletalAnimation::interpolateValues(float currentTime, Bone* animatedObject, Animated type, std::map<float, glm::vec3>& mapToInterpolate)
{
	if (mapToInterpolate.size() < 2)
		return;
	auto itPair = getProperIterators(currentTime, mapToInterpolate);
	auto leftKeyFrame = itPair.first;
	auto rightKeyFrame = itPair.second;

	if (rightKeyFrame == mapToInterpolate.end())
		return;

	float time = rightKeyFrame->first - leftKeyFrame->first;
	currentTime = currentTime - leftKeyFrame->first;
	glm::vec3 mix = glm::mix(leftKeyFrame->second, rightKeyFrame->second, currentTime / time);

	if (type == Animated::TRANSLATION) {
		animatedObject->localTransform.setPosition(mix);
	}
	else if (type == Animated::SCALE) {
		animatedObject->localTransform.setScale(mix);
	}
	else if (type == Animated::ROTATION) {
		animatedObject->localTransform.setRotationDegrees(mix);
	}
}

SkeletalAnimation::SkeletalAnimation(GraphNode* gameObject, std::string&& name) : Animation(gameObject, std::move(name)) {
	takeObjectsToAnimate(gameObject);
}

void SkeletalAnimation::update(float msec) {
	if (isPlaying && animatedBone != nullptr) {
		interpolate(animatedBone);
		currentTime += msec * speed;
	}

	if (currentTime >= endTime) {
		if (!looped) {
			isPlaying = false;
		}
		currentTime = 0.0f;
	}
}

SerializableType SkeletalAnimation::getSerializableType() {
	return SSkeletalAnimation;
}

void SkeletalAnimation::deserialize(Json::Value& root, Serializer* serializer) {
	Animation::deserialize(root, serializer);
	takeObjectsToAnimate(gameObject);
}

void SkeletalAnimation::renderGui() {
	//Animation::renderGui();
	ImGui::Text("If anim doesn't work, try reload.");
	if (ImGui::Button("Reload")) {
		takeObjectsToAnimate(gameObject);
	}
}

void SkeletalAnimation::setRootBone(Bone* animatedObject)
{
	animatedBone = animatedObject;
}

void SkeletalAnimation::takeObjectsToAnimate(GraphNode* objectToAnimate) {
	if (objectToAnimate) {
		AnimatedModel* model = dynamic_cast<AnimatedModel*>(gameObject->getMesh());
		if (model) {
			setRootBone(model->getBoneRoot());
		}
	}
}
