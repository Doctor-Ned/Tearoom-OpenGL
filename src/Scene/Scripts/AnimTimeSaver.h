#pragma once
#include "Scene/Components/KeyFrameAnimation.h"
#include "Scene/Components/LightComponents/Sun.h"
#include "Serialization/Serializer.h"

class SoundSource;

class AnimTimeSaver : public Component
{
protected:
	friend class Serializer;
	AnimTimeSaver(){}
	void renderGui() override;
	Sun *sun = nullptr;
	KeyFrameAnimation *animation = nullptr;
	SoundSource *sound = nullptr;
	int targetTime;
	bool startedPlaying;
	bool retargetAllowed;
	bool disableCollider = false;
	int ID;
public:
	void reset();
	int getID();
	Sun *getSun();
	KeyFrameAnimation *getAnimation();
	bool getDisableCollider();
	void setSun(Sun *sun);
	void setAnimation(KeyFrameAnimation *animation);
	void setDisableCollider(bool disableCollider);
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer *serializer) override;
	void deserialize(Json::Value &root, Serializer* serializer) override;
	AnimTimeSaver(GraphNode* _gameObject);
	void update(float msec) override;
};
