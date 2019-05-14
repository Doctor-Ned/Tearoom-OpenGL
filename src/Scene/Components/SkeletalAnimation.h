#ifndef SKELETAL_ANIMATION_H
#define SKELETAL_ANIMATION_H
#include "Scene/Components/Animation.h"
#include "Mesh/Bone.h"

class SkeletalAnimation : public Animation 
{
private:
	Bone* animatedBone = nullptr;
	void takeObjectsToAnimate(GraphNode* objectToAnimate);
	void interpolate(Bone* animatedObject);
	void interpolateValues(float currentTime, Bone* animatedObject, anim::Animated type, anim::animMap& mapToInterpolate);
	SkeletalAnimation() {}
	friend class Serializer;
public:
	SkeletalAnimation(GraphNode* gameObject, std::string&& name = "Animation");
	SerializableType getSerializableType() override;
	void update(float msec) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	void renderGui() override;
	void setRootBone(Bone* animatedObject);
};
#endif