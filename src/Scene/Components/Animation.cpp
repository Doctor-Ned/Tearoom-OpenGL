#include "Scene/Components/Animation.h"
#include "Scene/GraphNode.h"
#include <iostream>
#include "Scene/GameManager.h"

Animation::Animation(GraphNode* gameObject, std::string&& _name) : Component(gameObject), name(_name)
{
	addKeyFrame(gameObject, TRANSLATION, 2.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
	addKeyFrame(gameObject, TRANSLATION, 0.0f, glm::vec3(0));
	addKeyFrame(gameObject, TRANSLATION, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	addKeyFrame(gameObject, TRANSLATION, 3.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	addKeyFrame(gameObject, TRANSLATION, 3.0f, glm::vec3(5.0f, 2.0f, 0.0f));
	addKeyFrame(gameObject, TRANSLATION, 4.0f, glm::vec3(0));

	addKeyFrame(gameObject, SCALE, 0.0f, glm::vec3(1));
	addKeyFrame(gameObject, SCALE, 1.0f, glm::vec3(2.0f, 0.5f, 1.0f));
	addKeyFrame(gameObject, SCALE, 2.0f, glm::vec3(3.0f, 0.5f, 0.5f));
	addKeyFrame(gameObject, SCALE, 4.0f, glm::vec3(1));

	addKeyFrame(gameObject, ROTATION, 0.0f, glm::vec3(0));
	addKeyFrame(gameObject, ROTATION, 2.0f, glm::vec3(90.0f, 90.0f, 180.0f));
	addKeyFrame(gameObject, ROTATION, 4.0f, glm::vec3(0.0f, 0.0f, 360.0f));

	deleteKeyFrame(gameObject, ROTATION, 2.0f);

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
		play();
	}

	if(isPlaying)
	{
		for(auto it = objectAnimations.begin(); it != objectAnimations.end(); ++it)
		{
			translationInterpolation(currentTime, it->first, it->second.translation);
			rotationInterpolation(currentTime, it->first, it->second.rotation);
			scaleInterpolation(currentTime, it->first, it->second.scale);
		}
		
		currentTime += msec * speed;
	}
	
	if(currentTime >= endTime)
	{
		isPlaying = false;
		currentTime = 0.0f;
	}
}

void Animation::translationInterpolation(float currentTime, GraphNode* animatedObject, std::map<float, glm::vec3>& translation)
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
	else if (it2 == translation.end())
	{
		return;
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
	animatedObject->localTransform.setPosition(mix);
}

void Animation::scaleInterpolation(float currentTime, GraphNode* animatedObject, std::map<float, glm::vec3>& scale)
{
	if (scale.size() < 2)
		return;
	static std::map<float, glm::vec3>::iterator it;
	static std::map<float, glm::vec3>::iterator it2;

	static float time = 0.0f;
	if (currentTime == 0.0f)
	{
		it = scale.begin();
		it2 = std::next(it);
		time = it2->first - it->first;
	}
	else if (it2 == scale.end())
	{
		return;
	}
	else if (currentTime >= it2->first)
	{
		it = it2;
		it2 = std::next(it2);
		if (it2 == scale.end())
			return;
		time = it2->first - it->first;
	}
	currentTime = currentTime - it->first;
	glm::vec3 mix = glm::mix(it->second, it2->second, currentTime / time);
	
	gameObject->localTransform.setScale(mix);
}

void Animation::rotationInterpolation(float currentTime, GraphNode* animatedObject, std::map<float, glm::vec3>& rotation)
{
	if (rotation.size() < 2)
		return;
	static std::map<float, glm::vec3>::iterator it;
	static std::map<float, glm::vec3>::iterator it2;

	static float time = 0.0f;
	if (currentTime == 0.0f)
	{
		it = rotation.begin();
		it2 = std::next(it);
		time = it2->first - it->first;
	}
	else if (it2 == rotation.end())
	{
		return;
	}
	else if (currentTime >= it2->first)
	{
		it = it2;
		it2 = std::next(it2);
		if (it2 == rotation.end())
			return;
		time = it2->first - it->first;
	}
	currentTime = currentTime - it->first;

	glm::vec3 mix = glm::mix(it->second, it2->second, currentTime / time);
	animatedObject->localTransform.setRotationXDegrees(mix.x);
	animatedObject->localTransform.setRotationYDegrees(mix.y);
	animatedObject->localTransform.setRotationZDegrees(mix.z);
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

bool Animation::addKeyFrame(GraphNode* animatedNode, AnimType type, float time, glm::vec3 values)
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
	
	if(type == TRANSLATION)
	{
		it->second.translation[time] = values;
	}
	else if(type == SCALE)
	{
		it->second.scale[time] = values;
	}
	else if(type == ROTATION)
	{
		it->second.rotation[time] = values;
	}
	
	if (endTime < time)
		setEndTime(time);
	return true;
}

bool Animation::deleteKeyFrame(GraphNode* animatedNode, AnimType type, float time)
{
	auto it = objectAnimations.find(animatedNode);
	if (it == objectAnimations.end())
	{
		return false;
	}
	std::map<float, glm::vec3>::iterator keyFrameToDelete;
	if (type == TRANSLATION)
	{
		keyFrameToDelete = it->second.translation.find(time);
		if(keyFrameToDelete != it->second.translation.end())
		{
			it->second.translation.erase(keyFrameToDelete);
		}
	}
	else if (type == SCALE)
	{
		keyFrameToDelete = it->second.scale.find(time);
		if (keyFrameToDelete != it->second.scale.end())
		{
			it->second.scale.erase(keyFrameToDelete);
		}
	}
	else if (type == ROTATION)
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
		if (it->second.translation.size() >= 1)
		{
			auto it2 = std::prev(it->second.translation.end());
			if (endTime < it2->first)
			{
				endTime = it2->first;
			}
		}

		if (it->second.scale.size() >= 1)
		{
			auto it2 = std::prev(it->second.scale.end());
			if (endTime < it2->first)
			{
				endTime = it2->first;
			}
		}

		if (it->second.rotation.size() >= 1)
		{
			auto it2 = std::prev(it->second.rotation.end());
			if (endTime < it2->first)
			{
				endTime = it2->first;
			}
		}
	}
}
