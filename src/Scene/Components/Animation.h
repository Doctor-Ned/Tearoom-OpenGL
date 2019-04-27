#ifndef ANIMATION_H
#define ANIMATION_H

#include "Scene/Components/Component.h"
#include <map>

namespace anim {
	enum Animated
	{
		TRANSLATION,
		SCALE,
		ROTATION,
	};

	using animMap = std::map<float, glm::vec3>;

	struct ObjectAnimation
	{
		animMap translation;
		animMap scale;
		animMap rotation;
	};
}

class Animation : public Component
{
private:
	std::string name = "animation";
	float speed = 1.0f;
	float currentTime = 0.0f;
	float endTime = 0.0f;
	bool isPlaying = false;
	bool looped = false;

	void interpolateValues(float currentTime, GraphNode* animatedObject, anim::Animated type, anim::animMap& mapToInterpolate);
	std::pair<anim::animMap::iterator, anim::animMap::iterator> getProperIterators(float currentTime, anim::animMap& map);

	std::map<std::string, anim::ObjectAnimation> objectAnimations;
	std::vector<GraphNode*> objectsToAnimate;
	void setEndTime();
	void setCurrentTime(float);
	void setEndTime(float);
	void setIsPlaying(bool playing);
	void setSpeed(float _speed);
	void takeObjectsToAnimate(GraphNode* objectToAnimate);
	//#TODO: std::map serialization

	friend class Serializer;
	Animation() {}
public:
	Animation(GraphNode* gameObject, std::string&& name);
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	void update(float msec) override;
	~Animation() override;
	void renderGui() override;
	
	void play(float startTime = 0.0f, bool _looped = false);
	void stopPlaying();
	bool addKeyFrame(std::string&& gameObjectName, anim::Animated type, float time, glm::vec3 values);
	bool deleteKeyFrame(std::string&& gameObjectName, anim::Animated type, float time);
	void setName(std::string&& _name);
	std::string getName();
};

#endif
