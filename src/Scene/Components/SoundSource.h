#pragma once

#include "Scene/GraphNode.h"
#include "Scene/AssetManager.h"
#include "Scene/SoundSystem.h"
#include "Component.h"

class SoundSource : public Component {
public:
	SoundSource(std::string soundFile, GraphNode* node);
	~SoundSource();
	void update(float msec) override;
	void setVolume(float volume);
	float getVolume();
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	void play();
	void setLoop(bool loop);
	void stop();
	void set3D(bool is3d);
	void setSound(std::string sound);
	void setMinDistance(float minDistance);
	void setAttenuation(float attenuation);
protected:
	void renderGui() override;
	void update3dPos();
	std::string soundName = "";
	sf::Sound *sound = nullptr;
	bool loop = false;
	bool is3d = true;
	float volume = 1.0f, attenuation = 1.0f, minDistance = 0.2f;
	SoundSource() { }
	friend class Serializer;
};
