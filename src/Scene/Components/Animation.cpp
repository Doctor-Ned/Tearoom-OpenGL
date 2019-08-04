#include "Scene/Components/Animation.h"
#include "Scene/GraphNode.h"
#include <iostream>
#include "Scene/GameManager.h"
#include "Serialization/DataSerializer.h"
#include "Scene/AssetManager.h"

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
	root["animationDataName"] = animationDataName;
	
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
	animationDataName = root.get("animationDataName", animationDataName).asString();
	animationData = AssetManager::getInstance()->getAnimationData(std::move(animationDataName));
	takeObjectsToAnimate(gameObject);
}

KeyFrameIteratorPair Animation::getProperIterators(float currentTime, std::map<float, glm::vec3>& map)
{
	for(auto leftKeyFrame = map.begin(); leftKeyFrame != map.end(); ++leftKeyFrame)
	{
		std::map<float, glm::vec3>::iterator rightKeyFrame = std::next(leftKeyFrame);
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

void Animation::interpolateValues(float currentTime, GraphNode* animatedObject, Animated type, std::map<float, glm::vec3>& mapToInterpolate)
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

Animation::~Animation()
{
}

void Animation::renderGui()
{
	Component::renderGui();
	if (isComponentActive()) {
		ImGui::Text("__________________");
		static float startTime = 0.0f;
		static bool looped = false;

		ImGui::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, animationDataName.c_str());
		if(ImGui::Button("Choose animation data"))
		{
			showAnimationDataSelectionWindow = true;
		}
		if(showAnimationDataSelectionWindow)
		{
			if (ImGui::Begin("Animation data selection", &showAnimationDataSelectionWindow))
			{
				auto names = AssetManager::getInstance()->getAnimationDatasetsNames();
				for (auto&& name : names)
				{
					if (ImGui::Button(name.c_str()))
					{
						setAnimationData(AssetManager::getInstance()->getAnimationData(std::move(name)), std::move(name));
						showAnimationDataSelectionWindow = false;
					}
				}
				if (ImGui::Button("Close"))
				{
					showAnimationDataSelectionWindow = false;
				}
				ImGui::End();
			}
		}
		
		if (!animationData)
			return;
		
		ImGui::Checkbox("Looped", &looped);
		ImGui::InputFloat("startTime:", &startTime);
		ImGui::InputFloat("Speed", &speed);
		if (ImGui::Button("Play")) {
			play(startTime, looped);
		}
		if (ImGui::Button("Stop")) {
			stopPlaying();
		}
		/*
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
		if (ImGui::Button("Add/edit pos KeyFrame")) {
			glm::vec3 values = { x, y, z };
			addKeyFrame(str0, TRANSLATION, time, values);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete pos KeyFrame")) {
			glm::vec3 values = { x, y, z };
			deleteKeyFrame(str0, TRANSLATION, time);
		}

		if (ImGui::Button("Add/edit scale KeyFrame")) {
			glm::vec3 values = { x, y, z };
			addKeyFrame(str0, SCALE, time, values);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete scale KeyFrame")) {
			glm::vec3 values = { x, y, z };
			deleteKeyFrame(str0, SCALE, time);
		}

		if (ImGui::Button("Add/edit rot KeyFrame")) {
			glm::vec3 values = { x, y, z };
			addKeyFrame(str0, ROTATION, time, values);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete rot KeyFrame")) {
			glm::vec3 values = { x, y, z };
			deleteKeyFrame(str0, ROTATION, time);
		}*/

		for (auto animObjIt = animationData->keyFramesForObjects.begin(); animObjIt != animationData->keyFramesForObjects.end(); ++animObjIt) 
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
}

float Animation::getEndTime() {
	return endTime;
}

void Animation::animationGui(const char* animation, std::map<float, glm::vec3>& keyFrameMap)
{
	ImGui::Indent(15.0f);
	if (!keyFrameMap.empty())
	{
		ImGui::TextColored(ImVec4(0.0f, 0.2f, 1.0f, 1.0f), animation);
		auto it = keyFrameMap.begin();
		ImGui::Indent(15.0f);
		for (; it != keyFrameMap.end(); ++it)
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


bool Animation::getIsPlaying() {
	return isPlaying;
}

float Animation::getCurrentTime() {
	return currentTime;
}

AnimationData* Animation::getAnimationData()
{
	return animationData;
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
	if (!animationData)
		return false;
	if (time < 0)
		return false;
	if (isPlaying)
		return false;
	auto it = animationData->keyFramesForObjects.find(gameObjectName);
	if (it == animationData->keyFramesForObjects.end())
	{
		KeyFrameData keyFrameData;
		animationData->keyFramesForObjects.emplace(gameObjectName, keyFrameData);
	}
	auto keyFramesForObjectsIterator = animationData->keyFramesForObjects.find(gameObjectName);
	
	if(type == Animated::TRANSLATION)
	{
		keyFramesForObjectsIterator->second.translation[time] = values;
	}
	else if(type == Animated::SCALE)
	{
		keyFramesForObjectsIterator->second.scale[time] = values;
	}
	else if(type == Animated::ROTATION)
	{
		keyFramesForObjectsIterator->second.rotation[time] = values;
	}
	
	if (endTime < time)
		setEndTime(time);
	return true;
}

bool Animation::deleteKeyFrame(std::string&& gameObjectName, Animated type, float time)
{
	if (!animationData)
		return false;
	if (isPlaying)
		return false;

	auto it = animationData->keyFramesForObjects.find(gameObjectName);
	if (it == animationData->keyFramesForObjects.end())
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
		animationData->keyFramesForObjects.erase(it);
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

void Animation::setAnimationData(AnimationData* animationData, std::string&& name)
{
	animationDataName = name;
	this->animationData = animationData;
	takeObjectsToAnimate(gameObject);
	setEndTime();
}

void Animation::setEndTime()
{
	for (auto it = animationData->keyFramesForObjects.begin(); it != animationData->keyFramesForObjects.end(); ++it)
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
