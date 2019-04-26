#include "Scene/Components/Animation.h"
#include "Scene/GraphNode.h"
#include <iostream>
#include "Scene/GameManager.h"

using anim::Animated;
using anim::animMap;
using anim::ObjectAnimation;

Animation::Animation(GraphNode* gameObject, std::string&& _name) : Component(gameObject), name(_name)
{
	addKeyFrame(gameObject, Animated::TRANSLATION, 2.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
	addKeyFrame(gameObject, Animated::TRANSLATION, 0.0f, glm::vec3(0));
	addKeyFrame(gameObject, Animated::TRANSLATION, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	addKeyFrame(gameObject, Animated::TRANSLATION, 3.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	addKeyFrame(gameObject, Animated::TRANSLATION, 3.0f, glm::vec3(5.0f, 2.0f, 0.0f));
	addKeyFrame(gameObject, Animated::TRANSLATION, 4.0f, glm::vec3(0));

	addKeyFrame(gameObject, Animated::SCALE, 0.0f, glm::vec3(1));
	addKeyFrame(gameObject, Animated::SCALE, 1.0f, glm::vec3(2.0f, 0.5f, 1.0f));
	addKeyFrame(gameObject, Animated::SCALE, 2.0f, glm::vec3(3.0f, 0.5f, 0.5f));
	addKeyFrame(gameObject, Animated::SCALE, 4.0f, glm::vec3(1));

	addKeyFrame(gameObject, Animated::ROTATION, 0.0f, glm::vec3(0));
	addKeyFrame(gameObject, Animated::ROTATION, 2.0f, glm::vec3(90.0f, 90.0f, 180.0f));
	addKeyFrame(gameObject, Animated::ROTATION, 4.0f, glm::vec3(0.0f, 0.0f, 360.0f));

	deleteKeyFrame(gameObject, Animated::ROTATION, 2.0f);

	setSpeed(0.5f);
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

	return root;
}

void Animation::deserialize(Json::Value& root, Serializer* serializer)
{
	Component::deserialize(root, serializer);
	setName(root["name"].asString());
	setCurrentTime(root["currentTime"].asFloat());
	setEndTime(root["endTime"].asFloat());
	setIsPlaying(root["isPlaying"].asBool());
}

void Animation::update(float msec)
{
	if(!isPlaying && GameManager::getInstance()->getKeyState(GLFW_KEY_1))
	{
		play(true);
	}

	if(GameManager::getInstance()->getKeyState(GLFW_KEY_2))
	{
		stopPlaying();
	}

	if(isPlaying)
	{
		for(auto it = objectAnimations.begin(); it != objectAnimations.end(); ++it)
		{	//first -> animatedObject, second -> map
			interpolateValues(currentTime, it->first, Animated::TRANSLATION, it->second.translation);
			interpolateValues(currentTime, it->first, Animated::SCALE, it->second.scale);
			interpolateValues(currentTime, it->first, Animated::ROTATION, it->second.rotation);
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
	std::map<float, glm::vec3>::iterator leftKeyFrame;
	std::map<float, glm::vec3>::iterator rightKeyFrame;
	getProperIterators(currentTime, leftKeyFrame, rightKeyFrame, mapToInterpolate);

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
		gameObject->localTransform.setScale(mix);
	}
	else if (type == Animated::ROTATION)
	{
		animatedObject->localTransform.setRotationDegrees(mix);
	}
}

void Animation::getProperIterators(float currentTime, animMap::iterator& keyFrame1,
	animMap::iterator& keyFrame2, animMap& map)
{
	for(auto it = map.begin(); it != map.end(); ++it)
	{
		auto it2 = std::next(it);
		if(it2 == map.end())
		{
			return;
		}
		if(currentTime >= it->first && currentTime < it2->first)
		{
			keyFrame1 = it;
			keyFrame2 = it2;
			break;
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

void Animation::play(bool _looped)
{
	isPlaying = true;
	looped = _looped;
}

void Animation::stopPlaying()
{
	isPlaying = false;
	currentTime = 0.0f;
	looped = false;
}

bool Animation::addKeyFrame(GraphNode* animatedNode, Animated type, float time, glm::vec3 values)
{
	if (time < 0)
		return false;
	auto it = objectAnimations.find(animatedNode);
	if (it == objectAnimations.end())
	{
		ObjectAnimation anim;
		objectAnimations.emplace(animatedNode, anim);
	}
	it = objectAnimations.find(animatedNode);
	
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

bool Animation::deleteKeyFrame(GraphNode* animatedNode, Animated type, float time)
{
	auto it = objectAnimations.find(animatedNode);
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

void Animation::setName(std::string&& _name)
{
	name = _name;
}

void Animation::setEndTime()
{
	for (auto it = objectAnimations.begin(); it != objectAnimations.end(); ++it)
	{
		if (!it->second.translation.empty())
		{
			auto it2 = std::prev(it->second.translation.end());
			if (endTime < it2->first)
			{
				endTime = it2->first;
			}
		}

		if (!it->second.translation.empty())
		{
			auto it2 = std::prev(it->second.scale.end());
			if (endTime < it2->first)
			{
				endTime = it2->first;
			}
		}

		if (!it->second.translation.empty())
		{
			auto it2 = std::prev(it->second.rotation.end());
			if (endTime < it2->first)
			{
				endTime = it2->first;
			}
		}
	}
}
