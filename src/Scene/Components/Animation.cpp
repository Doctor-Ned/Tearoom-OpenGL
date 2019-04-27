#include "Scene/Components/Animation.h"
#include "Scene/GraphNode.h"
#include <iostream>
#include "Scene/GameManager.h"
#include "Serialization/DataSerializer.h"

using anim::Animated;
using anim::animMap;
using anim::ObjectAnimation;
using keyFramePair = std::pair<anim::animMap::iterator, anim::animMap::iterator>;
Animation::Animation(GraphNode* gameObject, std::string&& _name) : Component(gameObject, _name)
{
	takeObjectsToAnimate(gameObject);
	/*addKeyFrame(gameObject->getName(), Animated::TRANSLATION, 2.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
	addKeyFrame(gameObject->getName(), Animated::TRANSLATION, 0.0f, glm::vec3(0));
	addKeyFrame(gameObject->getName(), Animated::TRANSLATION, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	addKeyFrame(gameObject->getName(), Animated::TRANSLATION, 3.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	addKeyFrame(gameObject->getName(), Animated::TRANSLATION, 3.0f, glm::vec3(5.0f, 2.0f, 0.0f));
	addKeyFrame(gameObject->getName(), Animated::TRANSLATION, 4.0f, glm::vec3(0));

	addKeyFrame(gameObject->getName(), Animated::SCALE, 0.0f, glm::vec3(1));
	addKeyFrame(gameObject->getName(), Animated::SCALE, 1.0f, glm::vec3(2.0f, 0.5f, 1.0f));
	addKeyFrame(gameObject->getName(), Animated::SCALE, 2.0f, glm::vec3(3.0f, 0.5f, 0.5f));
	addKeyFrame(gameObject->getName(), Animated::SCALE, 4.0f, glm::vec3(1));

	addKeyFrame(gameObject->getName(), Animated::ROTATION, 0.0f, glm::vec3(0));
	addKeyFrame(gameObject->getName(), Animated::ROTATION, 2.0f, glm::vec3(90.0f, 90.0f, 180.0f));
	addKeyFrame(gameObject->getName(), Animated::ROTATION, 4.0f, glm::vec3(0.0f, 0.0f, 360.0f));

	deleteKeyFrame(gameObject->getName(), Animated::ROTATION, 4.0f);

	setSpeed(0.5f);*/
}

SerializableType Animation::getSerializableType()
{
	return SAnimation;
}

Json::Value Animation::serialize(Serializer* serializer)
{
	Json::Value root = Component::serialize(serializer);
	root["currentTime"] = currentTime;
	root["endTime"] = endTime;
	root["isPlaying"] = isPlaying;
	root["objectAnimations"] = DataSerializer::serializeObjectAnimationsMap(objectAnimations);
	
	return root;
}

void Animation::deserialize(Json::Value& root, Serializer* serializer)
{
	Component::deserialize(root, serializer);
	setCurrentTime(root["currentTime"].asFloat());
	setEndTime(root["endTime"].asFloat());
	setIsPlaying(root["isPlaying"].asBool());
	setObjectAnimations(DataSerializer::deserializeObjectAnimationsMap(root["objectAnimations"]));
	takeObjectsToAnimate(gameObject);
}

void Animation::update(float msec)
{
	if(!isPlaying && GameManager::getInstance()->getKeyState(GLFW_KEY_1))
	{
		//play(0.5f, true);
		play();
	}

	if(GameManager::getInstance()->getKeyState(GLFW_KEY_2))
	{
		stopPlaying();
	}

	if(isPlaying)
	{
		for(GraphNode* gameObject: objectsToAnimate)
		{
			auto animForGameObject = objectAnimations.find(gameObject->getName());
			if(animForGameObject != objectAnimations.end())
			{
				interpolateValues(currentTime, gameObject, Animated::TRANSLATION, animForGameObject->second.translation);
				interpolateValues(currentTime, gameObject, Animated::SCALE, animForGameObject->second.scale);
				interpolateValues(currentTime, gameObject, Animated::ROTATION, animForGameObject->second.rotation);
			}
		}
		currentTime += msec * speed;
	}
	
	if(currentTime >= endTime)
	{
		if(!looped)
		{
			isPlaying = false;
		}
		currentTime = 0.0f;
	}
}

void Animation::interpolateValues(float currentTime, GraphNode* animatedObject, Animated type, std::map<float, glm::vec3>& mapToInterpolate)
{
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

	if (type == Animated::TRANSLATION)
	{
		animatedObject->localTransform.setPosition(mix);
	}
	else if (type == Animated::SCALE)
	{
		animatedObject->localTransform.setScale(mix);
	}
	else if (type == Animated::ROTATION)
	{
		animatedObject->localTransform.setRotationDegrees(mix);
	}
}

keyFramePair Animation::getProperIterators(float currentTime, animMap& map)
{
	for(auto leftKeyFrame = map.begin(); leftKeyFrame != map.end(); ++leftKeyFrame)
	{
		auto rightKeyFrame = std::next(leftKeyFrame);
		if(rightKeyFrame == map.end())
		{
			return { leftKeyFrame, rightKeyFrame };
		}
		if(currentTime >= leftKeyFrame->first && currentTime < rightKeyFrame->first)
		{
			return { leftKeyFrame, rightKeyFrame };
		}
	}
}

Animation::~Animation()
{
}

void Animation::renderGui()
{
	Component::renderGui();
}

void Animation::play(float startTime, bool _looped)
{
	isPlaying = true;
	currentTime = startTime;
	looped = _looped;
}

void Animation::stopPlaying()
{
	isPlaying = false;
	currentTime = 0.0f;
	looped = false;
}

bool Animation::addKeyFrame(std::string&& gameObjectName, Animated type, float time, glm::vec3 values)
{
	if (time < 0)
		return false;
	auto it = objectAnimations.find(gameObjectName);
	if (it == objectAnimations.end())
	{
		ObjectAnimation anim;
		objectAnimations.emplace(gameObjectName, anim);
	}
	it = objectAnimations.find(gameObjectName);
	
	if(type == Animated::TRANSLATION)
	{
		it->second.translation[time] = values;
	}
	else if(type == Animated::SCALE)
	{
		it->second.scale[time] = values;
	}
	else if(type == Animated::ROTATION)
	{
		it->second.rotation[time] = values;
	}
	
	if (endTime < time)
		setEndTime(time);
	return true;
}

bool Animation::deleteKeyFrame(std::string&& gameObjectName, Animated type, float time)
{
	auto it = objectAnimations.find(gameObjectName);
	if (it == objectAnimations.end())
	{
		return false;
	}
	std::map<float, glm::vec3>::iterator keyFrameToDelete;
	if (type == Animated::TRANSLATION)
	{
		keyFrameToDelete = it->second.translation.find(time);
		if(keyFrameToDelete != it->second.translation.end())
		{
			it->second.translation.erase(keyFrameToDelete);
		}
	}
	else if (type == Animated::SCALE)
	{
		keyFrameToDelete = it->second.scale.find(time);
		if (keyFrameToDelete != it->second.scale.end())
		{
			it->second.scale.erase(keyFrameToDelete);
		}
	}
	else if (type == Animated::ROTATION)
	{
		keyFrameToDelete = it->second.rotation.find(time);
		if (keyFrameToDelete != it->second.rotation.end())
		{
			it->second.rotation.erase(keyFrameToDelete);
		}
	}
	setEndTime();
	return true;
}

void Animation::setCurrentTime(float curr)
{
	currentTime = curr;
}

void Animation::setEndTime(float end)
{
	endTime = end;
}

void Animation::setIsPlaying(bool playing)
{
	isPlaying = playing;
}

void Animation::setSpeed(float _speed)
{
	speed = _speed;
}

std::string Animation::getName()
{
	return name;
}

void Animation::takeObjectsToAnimate(GraphNode* objectToAnimate)
{
	objectsToAnimate.push_back(objectToAnimate);
	for(GraphNode* node : objectToAnimate->getChildren())
	{
		takeObjectsToAnimate(node);
	}
}

void Animation::setObjectAnimations(std::map<std::string, anim::ObjectAnimation>&& map)
{
	objectAnimations = map;
}


void Animation::setEndTime()
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
