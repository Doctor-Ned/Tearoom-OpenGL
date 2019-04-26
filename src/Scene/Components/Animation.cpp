#include "Scene/Components/Animation.h"
#include "Scene/GraphNode.h"
#include <iostream>
#include "Scene/GameManager.h"

void Animation::setEndTime()
{
	for(auto it = objectAnimations.begin(); it != objectAnimations.end(); ++it)
	{
		if(it->second.translation.size() >=2)
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

Animation::Animation(GraphNode* gameObject, std::string&& _name) : Component(gameObject), name(_name)
{
	std::map<float, glm::vec3> translation;
	std::map<float, glm::vec3> scale;
	std::map<float, glm::vec3> rotation;
	translation.emplace(2.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
	translation.emplace(0.0f, glm::vec3(0));
	translation.emplace(1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	translation.emplace(3.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	translation.emplace(4.0f, glm::vec3(0));


	scale.emplace(0.0f, glm::vec3(1));
	scale.emplace(1.0f, glm::vec3(2.0f, 0.5f, 1.0f));
	scale.emplace(2.0f, glm::vec3(3.0f, 0.5f, 0.5f));
	scale.emplace(4.0f, glm::vec3(1));

	rotation.emplace(0.0f, glm::vec3(0));
	rotation.emplace(2.0f, glm::vec3(90.0f, 90.0f, 0.0f));
	rotation.emplace(4.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	ObjectAnimation anim = { translation, scale, rotation };
	objectAnimations.emplace(gameObject, anim);

	setSpeed(0.5f);
	setEndTime();
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
		for(auto it = objectAnimations.begin(); it != objectAnimations.end(); ++it)
		{
			translationInterpolation(currentTime, it->first, it->second.translation);
			//rotationInterpolation(currentTime, it->first, it->second.rotation);
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
	
	static glm::vec3 mix(0);
	//if(mix != glm::vec3(0))
	//{
	//	gameObject->localTransform.scale(glm::vec3(1) / mix);
	//}
	//mix = glm::mix(it->second, it2->second, currentTime / time);
	//
	//gameObject->localTransform.scale(mix);
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

	//if (rotVec != glm::vec3(0))
	//{
	//	gameObject->localTransform.rotate(-rotVec.x, glm::vec3(1.0f, 0.0f, 0.0f));
	//	gameObject->localTransform.rotate(-rotVec.y, glm::vec3(0.0f,1.0f, 0.0f));
	//	gameObject->localTransform.rotate(-rotVec.z, glm::vec3(0.0f, 0.0f, 1.0f));
	//}
	//rotVec = glm::mix(it->second, it2->second, currentTime / time);

	//gameObject->localTransform.rotate(rotVec.x, glm::vec3(1.0f, 0.0f, 0.0f));
	//gameObject->localTransform.rotate(rotVec.y, glm::vec3(0.0f, 1.0f, 0.0f));
	//gameObject->localTransform.rotate(rotVec.z, glm::vec3(0.0f, 0.0f, 1.0f));
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
	//if (time < 0)
	//	return false;
	//auto result = translation.emplace(time, position);
	//return result.second; //if emplacing took place it returns true
	return false;
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


