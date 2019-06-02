#pragma once
#include "Scene/Components/KeyFrameAnimation.h"
#include "Scene/Components/LightComponents/Sun.h"

class AnimTimeSaver : public Component
{
protected:
	friend class Serializer;
	AnimTimeSaver(){}
	void renderGui() override;
	Sun *sun = nullptr;
	KeyFrameAnimation *animation = nullptr;
	int targetTime;
	bool startedPlaying;
	bool retargetAllowed;
	bool applyActivity = true;
public:
	void reset();
	Sun *getSun();
	KeyFrameAnimation *getAnimation();
	bool getApplyActivity();
	void setSun(Sun *sun);
	void setAnimation(KeyFrameAnimation *animation);
	void setApplyActivity(bool applyActivity);
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer *serializer) override;
	void deserialize(Json::Value &root, Serializer* serializer) override;
	AnimTimeSaver(GraphNode* _gameObject);
	void update(float msec) override;
};