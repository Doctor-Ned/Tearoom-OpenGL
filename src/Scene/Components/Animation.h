#ifndef ANIMATION_H
#define ANIMATION_H
#include "Scene/Components/Component.h"
#include <set>
#include <map>
struct TranslationKeyFrame
{
	glm::vec3 translation;
	float time;
};
class Animation : public Component
{
private:
	float currentTime = 0.0f;
	float endTime = 0.0f;
	bool isPlaying = false;
	std::map<float, glm::vec3> translation;
	void translationInterpolation(float currentTime);
public:
	Animation(GraphNode* gameObject);
	Animation(){}
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	void update(float msec) override;
	~Animation() override;
	void renderGui() override;
	void play();
	bool addKeyFrame(float time, glm::vec3 position);
	void setCurrentTime(float);
	void setEndTime(float);
	void setIsPlaying(bool playing);
};
#endif
