#ifndef ANIMATION_H
#define ANIMATION_H
#include "Scene/Components/Component.h"
#include <map>
struct ObjectAnimation
{
	std::map<float, glm::vec3> translation;
	std::map<float, glm::vec3> scale;
	std::map<float, glm::vec3> rotation;
};
class Animation : public Component
{
private:
	std::string name = "animation";
	float speed = 1.0f;
	float currentTime = 0.0f;
	float endTime = 0.0f;
	bool isPlaying = false;
	
	void translationInterpolation(float currentTime, GraphNode* animatedObject, std::map<float, glm::vec3>& translation);
	void scaleInterpolation(float currentTime, GraphNode* animatedObject, std::map<float, glm::vec3>& scale);
	void rotationInterpolation(float currentTime, GraphNode* animatedObject, std::map<float, glm::vec3>& rotation);

	std::map<GraphNode*, ObjectAnimation> objectAnimations;
	void setEndTime();
public:
	Animation(GraphNode* gameObject, std::string&& name);
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
