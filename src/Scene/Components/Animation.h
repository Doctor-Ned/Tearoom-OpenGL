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
		bool empty() const
		{
			if (!translation.empty())
				return false;
			if (!rotation.empty())
				return false;
			if (!rotation.empty())
				return false;

			return true;
		}
	};
}

class Animation : public Component
{
private:
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
	void setLooped(bool);
	void takeObjectsToAnimate(GraphNode* objectToAnimate);
	//#TODO: std::map serialization
	void setObjectAnimations(std::map<std::string, anim::ObjectAnimation>&& map);

	friend class Serializer;
	Animation() {}
public:
	Animation(GraphNode* gameObject, std::string&& name = "Animation");
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	void update(float msec) override;
	~Animation() override;
	void renderGui() override;
	void animationGui(static const char* animation, anim::animMap& map);
	
	void play(float startTime = 0.0f, bool _looped = false);
	void stopPlaying();
	bool addKeyFrame(std::string&& gameObjectName, anim::Animated type, float time, glm::vec3 values);
	bool deleteKeyFrame(std::string&& gameObjectName, anim::Animated type, float time);
	std::string getName();
};

#endif
