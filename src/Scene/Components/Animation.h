#ifndef ANIMATION_H
#define ANIMATION_H

#include "Scene/Components/Component.h"
#include "Scene/AnimationData.h"
#include <map>

enum Animated
{
	TRANSLATION,
	SCALE,
	ROTATION,
};

using KeyFrameIteratorPair = std::pair<std::map<float, glm::vec3>::iterator, std::map<float, glm::vec3>::iterator>;
class Animation abstract : public Component
{
protected:
	float speed = 1.0f;
	float currentTime = 0.0f;
	float endTime = 0.0f;
	bool isPlaying = false;
	bool looped = false;
	bool showAnimationDataSelectionWindow = false;
	std::string animationDataName = "Empty";
	AnimationData* animationData = nullptr;

	KeyFrameIteratorPair getProperIterators(float currentTime, std::map<float, glm::vec3>& map);
	virtual void interpolateValues(float currentTime, GraphNode* animatedObject, Animated type, std::map<float, glm::vec3>& mapToInterpolate);

	void setEndTime();
	void setCurrentTime(float);
	void setEndTime(float);
	void setIsPlaying(bool playing);
	void setSpeed(float _speed);
	void setLooped(bool);
	virtual void takeObjectsToAnimate(GraphNode* objectToAnimate) = 0;
	void animationGui(static const char* animation, std::map<float, glm::vec3>& keyFrameMap);
	friend class Serializer;
	Animation() {}
public:
	virtual void update(float msec) = 0;
	virtual SerializableType getSerializableType() = 0;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;

	Animation(GraphNode* gameObject, std::string&& name = "Animation");
	virtual ~Animation();
	
	void renderGui() override;

	float getEndTime();
	bool getIsPlaying();
	float getCurrentTime();
	AnimationData* getAnimationData();
	void setAnimationData(AnimationData* animationData, std::string&& name);

	void play(float startTime = 0.0f, bool _looped = false);
	void stopPlaying();
	virtual bool addKeyFrame(std::string&& gameObjectName, Animated type, float time, glm::vec3 values);
	virtual bool deleteKeyFrame(std::string&& gameObjectName, Animated type, float time);
};

#endif
