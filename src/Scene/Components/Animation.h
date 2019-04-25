#ifndef ANIMATION_H
#define ANIMATION_H
#include "Scene/Components/Component.h"
#include <map>

class Animation : public Component
{
private:
	float speed = 1.0f;
	float currentTime = 0.0f;
	float endTime = 0.0f;
	bool isPlaying = false;
	glm::vec3 rotVec;
	std::map<float, glm::vec3> translation;
	std::map<float, glm::vec3> scale;
	std::map<float, glm::vec3> rotation;
	void translationInterpolation(float currentTime);
	void scaleInterpolation(float currentTime);
	void rotationInterpolation(float currentTime);
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
	void setSpeed(float _speed);
};
#endif
