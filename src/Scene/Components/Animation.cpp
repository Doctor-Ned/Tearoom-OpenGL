#include "Scene/Components/Animation.h"
#include "Scene/GraphNode.h"
#include <iostream>
#include "Scene/GameManager.h"
#include "Serialization/DataSerializer.h"

using anim::Animated;
using anim::animMap;
using anim::ObjectAnimation;

Animation::Animation(GraphNode* gameObject, std::string&& _name) : Component(gameObject, _name)
{
	
}

Json::Value Animation::serialize(Serializer* serializer)
{
	Json::Value root = Component::serialize(serializer);
	root["currentTime"] = currentTime;
	root["endTime"] = endTime;
	root["isPlaying"] = isPlaying;
	root["looped"] = looped;
	root["speed"] = speed;
	root["objectAnimations"] = DataSerializer::serializeObjectAnimationsMap(objectAnimations);
	
	return root;
}

void Animation::deserialize(Json::Value& root, Serializer* serializer)
{
	Component::deserialize(root, serializer);
	setCurrentTime(root["currentTime"].asFloat());
	setEndTime(root["endTime"].asFloat());
	setIsPlaying(root["isPlaying"].asBool());
	setLooped(root["looped"].asBool());
	setSpeed(root["speed"].asFloat());
	setObjectAnimations(DataSerializer::deserializeObjectAnimationsMap(root["objectAnimations"]));
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

std::map<std::string, anim::ObjectAnimation> Animation::getObjectAnimations()
{
	return objectAnimations;
}

void Animation::setObjectAnimation(std::map<std::string, anim::ObjectAnimation> map)
{
	objectAnimations = map;
	setEndTime();
}

Animation::~Animation()
{
}

void Animation::renderGui()
{
	Component::renderGui();
	ImGui::Text("__________________");
	static float startTime = 0.0f;
	static bool looped = false;
	ImGui::Checkbox("Looped", &looped);
	ImGui::InputFloat("startTime:", &startTime);
	ImGui::InputFloat("Speed", &speed);
	if(ImGui::Button("Play"))
	{
		play(startTime, looped);
	}
	if(ImGui::Button("Stop"))
	{
		stopPlaying();
	}
	ImGui::Text("Edition");
	static float time = 0.0f;
	static float x = 0.0f;
	static float y = 0.0f;
	static float z = 0.0f;
	static char str0[128] = "Object to animate";
	ImGui::InputText("AnimatedObject", str0, IM_ARRAYSIZE(str0));
	ImGui::InputFloat("time:", &time);
	ImGui::InputFloat("fixed X:", &x);
	ImGui::InputFloat("fixed Y:", &y);
	ImGui::InputFloat("fixed Z:", &z);
	if (ImGui::Button("Add/edit pos KeyFrame"))
	{
		glm::vec3 values = { x, y, z };
		addKeyFrame(str0, anim::TRANSLATION, time, values);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete pos KeyFrame"))
	{
		glm::vec3 values = { x, y, z };
		deleteKeyFrame(str0, anim::TRANSLATION, time);
	}

	if (ImGui::Button("Add/edit scale KeyFrame"))
	{
		glm::vec3 values = { x, y, z };
		addKeyFrame(str0, anim::SCALE, time, values);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete scale KeyFrame"))
	{
		glm::vec3 values = { x, y, z };
		deleteKeyFrame(str0, anim::SCALE, time);
	}

	if (ImGui::Button("Add/edit rot KeyFrame"))
	{
		glm::vec3 values = { x, y, z };
		addKeyFrame(str0, anim::ROTATION, time, values);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete rot KeyFrame"))
	{
		glm::vec3 values = { x, y, z };
		deleteKeyFrame(str0, anim::ROTATION, time);
	}


	for(auto animObjIt = objectAnimations.begin(); animObjIt != objectAnimations.end(); ++animObjIt)
	{
		std::string animatedObject = animObjIt->first;
		ImGui::Text("Object:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.6f, 1.0f), animatedObject.c_str());

		animationGui("Translation", animObjIt->second.translation);
		animationGui("Scale", animObjIt->second.scale);
		animationGui("Rotation", animObjIt->second.rotation);
	}
}

void Animation::animationGui(const char* animation, anim::animMap& map)
{
	ImGui::Indent(15.0f);
	if (!map.empty())
	{
		ImGui::TextColored(ImVec4(0.0f, 0.2f, 1.0f, 1.0f), animation);
		auto it = map.begin();
		ImGui::Indent(15.0f);
		for (; it != map.end(); ++it)
		{
			std::string time = std::to_string(it->first) + "s";
			ImGui::Text(time.c_str());
			ImGui::SameLine();
			ImGui::Text("[");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.8f, 0.0f, 0.0f, 1.0f), std::to_string(it->second.x).c_str());
			ImGui::SameLine();
			ImGui::Text(",");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), std::to_string(it->second.y).c_str());
			ImGui::SameLine();
			ImGui::Text(",");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 0.2f, 0.8f, 1.0f), std::to_string(it->second.z).c_str());
			ImGui::SameLine();
			ImGui::Text("]");
		}
		ImGui::Unindent(15.0f);
	}
	ImGui::Unindent(15.0f);
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
	if (isPlaying)
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
	if (isPlaying)
		return false;

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

	//removing empty animated object
	if(it->second.empty())
	{
		objectAnimations.erase(it);
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

void Animation::setLooped(bool val)
{
	looped = val;
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
