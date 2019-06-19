#include "SoundSource.h"
#include "Camera.h"
#include "Scene/Scenes/EditorScene.h"

SoundSource::SoundSource(std::string soundFile, GraphNode* node) : Component(node, "Sound source") {
	setSound(soundFile);
}

SoundSource::~SoundSource() {
	delete sound;
}

void SoundSource::update(float msec) {
	if(is3d) update3dPos();
}

void SoundSource::setVolume(float volume) {
	if (sound) {
		sound->setVolume(volume);
	}
	this->volume = volume;
}

float SoundSource::getVolume() {
	return volume;
}

SerializableType SoundSource::getSerializableType() {
	return SSoundSource;
}

Json::Value SoundSource::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["soundName"] = soundName;
	root["volume"] = volume;
	root["loop"] = loop;
	root["attenuation"] = attenuation;
	root["is3d"] = is3d;
	root["minDistance"] = minDistance;
	return root;
}

void SoundSource::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	soundName = root["soundName"].asString();
	setLoop(root["loop"].asBool());
	setVolume(root["volume"].asFloat());
	setMinDistance(root["minDistance"].asFloat());
	setAttenuation(root["attenuation"].asFloat());
	set3D(root["is3d"].asBool());
	setSound(soundName);
}

void SoundSource::play() {
	if (sound) {
		sound->play();
	}
}

void SoundSource::setLoop(bool loop) {
	if (sound) {
		sound->setLoop(loop);
	}
	this->loop = loop;
}

void SoundSource::stop() {
	if (sound) {
		sound->stop();
	}
}

void SoundSource::set3D(bool is3d) {
	this->is3d = is3d;
	if(sound) {
		sound->setRelativeToListener(!is3d);
		if(!is3d) {
			sound->setPosition(0.0f, 0.0f, 0.0f);
		} else {
			update3dPos();
		}
	}
}

void SoundSource::setSound(std::string soundFile) {
	soundName = soundFile;
	if (sound) {
		sound->stop();
		delete sound;
		sound = nullptr;
	}
	sound = SoundSystem::getSound(soundFile);
	set3D(is3d);
	setVolume(volume);
	setLoop(loop);
	setAttenuation(attenuation);
	setMinDistance(minDistance);
}

void SoundSource::setMinDistance(float minDistance) {
	this->minDistance = minDistance;
	if(sound) {
		sound->setMinDistance(minDistance);
	}
}

void SoundSource::setAttenuation(float attenuation) {
	this->attenuation = attenuation;
	if(sound) {
		sound->setAttenuation(attenuation);
	}
}

void SoundSource::renderGui() {
	if (active) {
		ImGui::Text(("Sound: '" + soundName + "'").c_str());
		EditorScene *editor = gameManager->getEditorScene();
		if (editor) {
			if (editor->soundSelectionCallback == nullptr && ImGui::Button("CHANGE SOUND")) {
				editor->soundSelectionCallback = [this](std::string soundName) {
					setSound(soundName);
				};
			}
		}
		bool loop = this->loop, is3d=this->is3d;
		float volume = this->volume, attenuation=this->attenuation, minDistance=this->minDistance;
		ImGui::Checkbox("3D", &is3d);
		ImGui::Checkbox("Loop", &loop);
		ImGui::DragFloat("Volume", &volume, 0.05f, 0.0f, 10.0f);
		ImGui::DragFloat("Attenuation", &attenuation, 0.05f, 0.0f, 100.0f);
		ImGui::DragFloat("Min distance", &minDistance, 0.05f, 0.0f, 10.0f);
		if (is3d != this->is3d) {
			set3D(is3d);
		}

		if (loop != this->loop) {
			setLoop(loop);
		}

		if (volume != this->volume) {
			setVolume(volume);
		}

		if (attenuation != this->attenuation) {
			setAttenuation(attenuation);
		}

		if (minDistance != this->minDistance) {
			setMinDistance(minDistance);
		}

		if (sound) {
			if (ImGui::Button("PLAY")) {
				sound->play();
			}
			ImGui::SameLine();
			if (ImGui::Button("STOP")) {
				sound->stop();
			}
		} else {
			ImGui::Text("NO SOUND AVAILABLE!");
		}
	}
}

void SoundSource::update3dPos() {
	if (sound) {
		glm::vec3 pos = gameObject->getPosition();
		sound->setPosition(pos.x, pos.y, pos.z);
	}
}
