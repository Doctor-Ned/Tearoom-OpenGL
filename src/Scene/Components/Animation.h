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
using keyFramePair = std::pair<anim::animMap::iterator, anim::animMap::iterator>;

class Animation abstract : public Component
{
protected:
	float speed = 1.0f;
	float currentTime = 0.0f;
	float endTime = 0.0f;
	bool isPlaying = false;
	bool looped = false;

	
	keyFramePair getProperIterators(float currentTime, anim::animMap& map);
	std::map<std::string, anim::ObjectAnimation> objectAnimations;
	
	void setEndTime();
	void setCurrentTime(float);
	void setEndTime(float);
	void setIsPlaying(bool playing);
	void setSpeed(float _speed);
	void setLooped(bool);
	void setObjectAnimations(std::map<std::string, anim::ObjectAnimation>&& map);

	void animationGui(static const char* animation, anim::animMap& map);
	friend class Serializer;
	Animation() {}
public:
	void update(float msec) = 0;
	SerializableType getSerializableType() = 0;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;

	Animation(GraphNode* gameObject, std::string&& name = "Animation");
	virtual ~Animation();
	
	void renderGui() override;
	
	void play(float startTime = 0.0f, bool _looped = false);
	void stopPlaying();
	bool addKeyFrame(std::string&& gameObjectName, anim::Animated type, float time, glm::vec3 values);
	bool deleteKeyFrame(std::string&& gameObjectName, anim::Animated type, float time);

	std::map<std::string, anim::ObjectAnimation> getObjectAnimations();
	void setObjectAnimation(std::map<std::string, anim::ObjectAnimation> map);
};

#endif
