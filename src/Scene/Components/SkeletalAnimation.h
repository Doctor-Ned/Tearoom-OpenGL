#ifndef SKELETAL_ANIMATION_H
#define SKELETAL_ANIMATION_H
#include "Scene/Components/Animation.h"
#include "Mesh/Bone.h"

using quatkeyFramePair = std::pair<std::map<float, glm::quat>::iterator, std::map<float, glm::quat>::iterator>;

struct SkeletalObjectAnimation
{
	anim::animMap translation;
	anim::animMap  scale;
	std::map<float, glm::quat> rotation;
	bool empty() const
	{
		if (!translation.empty())
			return false;
		if (!scale.empty())
			return false;
		if (!rotation.empty())
			return false;

		return true;
	}
};

class SkeletalAnimation : public Animation 
{
private:
	std::map<std::string, SkeletalObjectAnimation> objectAnimations;
	Bone* animatedBone = nullptr;
	void takeObjectsToAnimate(GraphNode* objectToAnimate);
	void interpolate(Bone* animatedObject);
	void interpolateValues(float currentTime, Bone* animatedObject, anim::Animated type, anim::animMap& mapToInterpolate);
	void interpolateValues(float currentTime, Bone* animatedObject, std::map<float, glm::quat>& mapToInterpolate);
	quatkeyFramePair getProperIteratorsQuat(float currentTime, std::map<float, glm::quat>& map);
	void setEndTime();
	void setEndTime(float end);
	SkeletalAnimation() {}
	friend class Serializer;
public:
	SkeletalAnimation(GraphNode* gameObject, std::string&& name = "Animation");
	SerializableType getSerializableType() override;
	void update(float msec) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	void renderGui() override;
	void setRootBone(Bone* animatedObject);
	std::map<std::string, SkeletalObjectAnimation> getObjectAnimations();
	void setObjectAnimation(std::map<std::string, SkeletalObjectAnimation> map);

	bool addKeyFrame(std::string&& gameObjectName, anim::Animated type, float time, glm::vec3 values);
	bool addKeyFrame(std::string&& gameObjectName, anim::Animated type, float time, glm::quat values);
	bool deleteKeyFrame(std::string&& gameObjectName, anim::Animated type, float time);
	
};
#endif