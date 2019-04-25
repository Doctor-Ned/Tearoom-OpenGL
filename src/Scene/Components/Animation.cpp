#include "Scene/Components/Animation.h"
#include "Scene/GraphNode.h"
#include <iostream>
#include "Scene/GameManager.h"

Animation::Animation(GraphNode* gameObject) : Component(gameObject)
{
	translation.emplace(2.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
	translation.emplace(0.0f, glm::vec3(0));
	translation.emplace(1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	translation.emplace(3.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	translation.emplace(4.0f, glm::vec3(1.0f, -2.5f, 1.0f));
	endTime = 4.0f;
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
	setCurrentTime(root["currentTime"].asFloat());
	setEndTime(root["endTime"].asFloat());
	setIsPlaying(root["isPlaying"].asBool());
}

void Animation::update(float msec)
{
	if(!isPlaying && GameManager::getInstance()->getKeyState(GLFW_KEY_1))
	{
		play();
	}

	if(isPlaying)
	{
		translationInterpolation(currentTime);
		currentTime += msec;
	}
	
	if(currentTime >= endTime)
	{
		isPlaying = false;
		currentTime = 0.0f;
	}
}

void Animation::translationInterpolation(float currentTime)
{
	if (translation.size() < 2)
		return;
	static std::map<float, glm::vec3>::iterator it;
	static std::map<float, glm::vec3>::iterator it2;
	static float time = 0.0f;
	if(currentTime == 0.0f)
	{
		it = translation.begin();
		it2 = std::next(it);
		time = it2->first - it->first;
	}
	else if(currentTime >= it2->first)
	{
		it = it2;
		it2 = std::next(it2);
		if (it2 == translation.end())
			return;
		time = it2->first - it->first;
	}
	currentTime = currentTime - it->first;
	glm::vec3 mix = glm::mix(it->second, it2->second, currentTime / time);
	gameObject->localTransform.setPosition(mix);
}

Animation::~Animation()
{
}

void Animation::renderGui()
{
	Component::renderGui();
}

void Animation::play()
{
	isPlaying = true;
}

bool Animation::addKeyFrame(float time, glm::vec3 position)
{
	if (time < 0)
		return false;
	auto result = translation.emplace(time, position);
	return result.second; //if emplacing took place it returns true
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


