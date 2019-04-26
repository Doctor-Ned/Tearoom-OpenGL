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
	void getProperIterators(float currentTime, anim::animMap::iterator& keyFrame1, anim::animMap::iterator& keyFrame2, anim::animMap& map);

	std::map<GraphNode*, anim::ObjectAnimation> objectAnimations;
	void setEndTime();
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


	void play(bool _looped = false);
	void stopPlaying();
	bool addKeyFrame(GraphNode* animatedNode, anim::Animated type, float time, glm::vec3 values);
	bool deleteKeyFrame(GraphNode* animatedNode, anim::Animated type, float time);
	void setCurrentTime(float);
	void setEndTime(float);
	void setIsPlaying(bool playing);
	void setSpeed(float _speed);
	void setName(std::string&& _name);
};

#endif
