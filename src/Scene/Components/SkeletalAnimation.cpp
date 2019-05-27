#include "SkeletalAnimation.h"
#include "Scene/GraphNode.h"
#include "Mesh/AnimatedModel.h"

using anim::Animated;
using anim::animMap;
using anim::ObjectAnimation;

void SkeletalAnimation::interpolate(Bone* animatedObject) {
	auto animForGameObject = objectAnimations.find(animatedObject->name);
	if (animForGameObject != objectAnimations.end()) {
		interpolateValues(currentTime, animatedObject, Animated::TRANSLATION, animForGameObject->second.translation);
		interpolateValues(currentTime, animatedObject, Animated::SCALE, animForGameObject->second.scale);
		interpolateValues(currentTime, animatedObject, animForGameObject->second.rotation);
	}

	for (auto& child : animatedObject->children) {
		interpolate(child);
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

void SkeletalAnimation::setObjectAnimation(std::map<std::string, SkeletalObjectAnimation> map)
{
	objectAnimations = map;
	takeObjectsToAnimate(gameObject);
	setEndTime();
}

bool SkeletalAnimation::addKeyFrame(std::string&& gameObjectName, anim::Animated type, float time, glm::vec3 values)
{
	if (time < 0)
		return false;
	if (isPlaying)
		return false;
	auto it = objectAnimations.find(gameObjectName);
	if (it == objectAnimations.end())
	{
		SkeletalObjectAnimation anim;
		objectAnimations.emplace(gameObjectName, anim);
	}
	it = objectAnimations.find(gameObjectName);

	if (type == Animated::TRANSLATION)
	{
		it->second.translation[time] = values;
	}
	else if (type == Animated::SCALE)
	{
		it->second.scale[time] = values;
	}

	if (endTime < time)
		setEndTime(time);
	return true;
}

bool SkeletalAnimation::addKeyFrame(std::string&& gameObjectName, anim::Animated type, float time, glm::quat values)
{
	if (time < 0)
		return false;
	if (isPlaying)
		return false;
	auto it = objectAnimations.find(gameObjectName);
	if (it == objectAnimations.end())
	{
		SkeletalObjectAnimation anim;
		objectAnimations.emplace(gameObjectName, anim);
	}
	it = objectAnimations.find(gameObjectName);

	if (type == Animated::ROTATION)
	{
		it->second.rotation[time] = values;
	}

	if (endTime < time)
		setEndTime(time);
	return true;
}

bool SkeletalAnimation::deleteKeyFrame(std::string&& gameObjectName, anim::Animated type, float time)
{
	if (isPlaying)
		return false;

	auto it = objectAnimations.find(gameObjectName);
	if (it == objectAnimations.end())
	{
		return false;
	}
	
	if (type == Animated::TRANSLATION)
	{
		std::map<float, glm::vec3>::iterator keyFrameToDelete = it->second.translation.find(time);
		if (keyFrameToDelete != it->second.translation.end())
		{
			it->second.translation.erase(keyFrameToDelete);
		}
	}
	else if (type == Animated::SCALE)
	{
		std::map<float, glm::vec3>::iterator keyFrameToDelete = it->second.scale.find(time);
		if (keyFrameToDelete != it->second.scale.end())
		{
			it->second.scale.erase(keyFrameToDelete);
		}
	}
	else if (type == Animated::ROTATION)
	{
		auto keyFrameToDelete = it->second.rotation.find(time);
		if (keyFrameToDelete != it->second.rotation.end())
		{
			it->second.rotation.erase(keyFrameToDelete);
		}
	}

	//removing empty animated object
	if (it->second.empty())
	{
		objectAnimations.erase(it);
	}
	setEndTime();
	return true;
}

SerializableType SkeletalAnimation::getSerializableType() {
	return SSkeletalAnimation;
}

void SkeletalAnimation::deserialize(Json::Value& root, Serializer* serializer) {
	//Animation::deserialize(root, serializer);
	takeObjectsToAnimate(gameObject);
}

void SkeletalAnimation::renderGui() {
	//Animation::renderGui();
	ImGui::Text("If anim doesn't work, try reload.");
	if (ImGui::Button("Reload")) {
		takeObjectsToAnimate(gameObject);
	}
}

void SkeletalAnimation::setRootBone(Bone* animatedObject) {
	animatedBone = animatedObject;
}

std::map<std::string, SkeletalObjectAnimation> SkeletalAnimation::getObjectAnimations()
{ 
	return objectAnimations;
}

void SkeletalAnimation::interpolateValues(float currentTime, Bone* animatedObject, Animated type, std::map<float, glm::vec3>& mapToInterpolate) {
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
	}
}

void SkeletalAnimation::interpolateValues(float currentTime, Bone* animatedObject,
	std::map<float, glm::quat>& mapToInterpolate)
{
	if (mapToInterpolate.size() < 2)
		return;
	quatkeyFramePair itPair = getProperIteratorsQuat(currentTime, mapToInterpolate);
	std::map<float, glm::quat>::iterator leftKeyFrame = itPair.first;
	std::map<float, glm::quat>::iterator rightKeyFrame = itPair.second;

	if (rightKeyFrame == mapToInterpolate.end())
		return;

	float time = rightKeyFrame->first - leftKeyFrame->first;
	currentTime = currentTime - leftKeyFrame->first;
	//glm::vec3 mix = glm::mix(leftKeyFrame->second, rightKeyFrame->second, currentTime / time);
	glm::quat mix = glm::lerp(leftKeyFrame->second, rightKeyFrame->second, currentTime / time);

	animatedObject->localTransform.setQuatRotation(mix);
}

void SkeletalAnimation::takeObjectsToAnimate(GraphNode* objectToAnimate) {
	if (objectToAnimate) {
		AnimatedModel* model = dynamic_cast<AnimatedModel*>(gameObject->getMesh());
		if (model) {
			setRootBone(model->getBoneRoot());
		}
	}
}

quatkeyFramePair SkeletalAnimation::getProperIteratorsQuat(float currentTime, std::map<float, glm::quat>& map)
{
	for (auto leftKeyFrame = map.begin(); leftKeyFrame != map.end(); ++leftKeyFrame)
	{
		auto rightKeyFrame = std::next(leftKeyFrame);
		if (rightKeyFrame == map.end())
		{
			return { leftKeyFrame, rightKeyFrame };
		}
		if (currentTime >= leftKeyFrame->first && currentTime < rightKeyFrame->first)
		{
			return { leftKeyFrame, rightKeyFrame };
		}
	}
}

void SkeletalAnimation::setEndTime(float end)
{
	endTime = end;
}

void SkeletalAnimation::setEndTime()
{
	for (auto it = objectAnimations.begin(); it != objectAnimations.end(); ++it)
	{
		if (it->second.translation.size() >= 2)
		{
			auto it2 = std::prev(it->second.translation.end());
			if (endTime < it2->first)
			{
				endTime = it2->first;
			}
		}

		if (it->second.scale.size() >= 2)
		{
			auto it2 = std::prev(it->second.scale.end());
			if (endTime < it2->first)
			{
				endTime = it2->first;
			}
		}

		if (it->second.rotation.size() >= 2)
		{
			auto it2 = std::prev(it->second.rotation.end());
			if (endTime < it2->first)
			{
				endTime = it2->first;
			}
		}
	}
}
