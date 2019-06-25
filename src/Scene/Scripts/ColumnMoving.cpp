#include "Scene/Scripts/ColumnMoving.h"
#include "Scene/Scenes/EditorScene.h"
#include "Serialization/Serializer.h"
#include "Scene/Components/SoundSource.h"

void ColumnMoving::update(float msec) {
	lastPos = gameObject->localTransform.getPosition();
	if (sun != nullptr) {
		glm::vec3 pos = gameObject->localTransform.getPosition();
		gameObject->localTransform.setPosition(glm::vec3(pos.x, getHeightForTime(sun->getTime()), pos.z));
		if(getFractionalPartOfHour(sun->getTime()) == 0.0f)
		{
			 enableSound = true;
		}

		if(getFractionalPartOfHour(sun->getTime()) != 0.0f && enableSound && lastPos - gameObject->localTransform.getPosition() != glm::vec3(0))
		{
			if (sound != nullptr)
			{
				sound->play();
			}
			enableSound = false;
		}
	}
}


void ColumnMoving::renderGui() {
	Component::renderGui();

	ImGui::Text("Sun source: ");
	ImGui::SameLine();
	if (sun == nullptr) {
		ImGui::Text("null");
	} else if (sun->getGameObject() == gameObject) {
		ImGui::Text("current node");
	} else {
		ImGui::Text(sun->getGameObject()->getName().c_str());
	}
	EditorScene *editor = GameManager::getInstance()->getEditorScene();
	if (editor != nullptr) {
		if (editor->nodeSelectionCallback == nullptr && ImGui::Button("Choose sun component")) {
			editor->nodeSelectionCallback = [this](GraphNode* node) {
				setSun(node->getComponent<Sun>());
			};
		}
	}

	ImGui::Text(("Sound acquired from: " + (sound == nullptr ? "None" : sound->getGameObject()->getName())).c_str());
	if (editor && editor->nodeSelectionCallback == nullptr) {
		if (ImGui::Button("CHOOSE SOUND OBJECT")) {
			editor->nodeSelectionCallback = [this](GraphNode *node) {
				sound = node->getComponent<SoundSource>();
			};
		}
	}

	static int hour = 0;
	static float height = 0;

	ImGui::InputInt("hour", &hour);
	ImGui::InputFloat("height", &height);

	if (ImGui::Button("Add/edit heightInHour")) {
		addHeightOnSelectedHour(hour, height);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete heightInHour")) {
		removeHeightOnSelectedHour(hour);
	}

	for (auto iterator : heightToHourMap) {
		ImGui::Text(std::to_string(iterator.first).c_str());
		ImGui::SameLine();
		ImGui::Text(std::to_string(iterator.second).c_str());
	}
}

float ColumnMoving::getFractionalPartOfHour(float hour) {
	float integralPartOfHour;
	return modf(hour, &integralPartOfHour);
}

float ColumnMoving::getHeightForTime(float time) {
	if(heightToHourMap.empty()) {
		return gameObject->localTransform.getPosition().y;
	}
	if(heightToHourMap.size() == 1) {
		return (*heightToHourMap.begin()).second;
	}
	std::pair<int, float> last;
	last.first = -1;
	std::pair<int, float> from, to;
	from.first = -1;
	to.first = -1;
	for(auto &pair : heightToHourMap) {
		if(abs(time - pair.first) < 0.01f) {
			return pair.second;
		}
		if(pair.first < time) {
			from = pair;
		} else if(pair.first > time) {
			from = last;
			to = pair;
			break;
		}
		last = pair;
	}
	if(from.first == -1) {
		return to.second;
	}
	if(to.first == -1) {
		return from.second;
	}
	if(abs(from.second - to.second) < 0.01f) {
		return from.second;
	}
	bool reverse = to.second < from.second;
	return Global::remap(time, static_cast<float>(from.first), static_cast<float>(to.first), reverse ? to.second : from.second, reverse ? from.second : to.second, reverse);
}

ColumnMoving::ColumnMoving(GraphNode* gameObject, std::string name) : Component(gameObject, name) {}

SerializableType ColumnMoving::getSerializableType() {
	return SColumnMoving;
}

Json::Value ColumnMoving::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["sun"] = serializer->serialize(sun);
	root["heightToHourMap"] = DataSerializer::serializeIntFloatMap(heightToHourMap);
	root["sound"] = serializer->serialize(sound);
	return root;
}

void ColumnMoving::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	sun = dynamic_cast<Sun*>(serializer->deserialize(root["sun"]).object);
	heightToHourMap = DataSerializer::deserializeIntFloatMap(root["heightToHourMap"]);
	sound = dynamic_cast<SoundSource*>(serializer->deserialize(root["sound"]).object);
}

void ColumnMoving::updateWorld() {}

bool ColumnMoving::isComponentActive() {
	return active;
}

void ColumnMoving::setComponentActive(bool active) {
	this->active = active;
}

void ColumnMoving::setSun(Sun* sun) {
	this->sun = sun;
}

void ColumnMoving::addHeightOnSelectedHour(int hour, float height) {
	heightToHourMap[hour] = height;
}

void ColumnMoving::removeHeightOnSelectedHour(int hour) {
	auto heightInSelectedHourIterator = heightToHourMap.find(hour);
	if (heightInSelectedHourIterator != heightToHourMap.end()) {
		heightToHourMap.erase(heightInSelectedHourIterator);
	}
}


ColumnMoving::~ColumnMoving() {

}

