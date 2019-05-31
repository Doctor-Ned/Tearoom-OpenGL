#ifndef KEYFRAME_ANIMATION_H
#define KEYFRAME_ANIMATION_H
#include "Animation.h"

class KeyFrameAnimation : public Animation
{
private:
	std::vector<GraphNode*> objectsToAnimate;

	void interpolateValues(float currentTime, GraphNode* animatedObject, anim::Animated type, anim::animMap& mapToInterpolate);
	void takeObjectsToAnimate(GraphNode* objectToAnimate);
	void interpolateObjects();
	KeyFrameAnimation() {}
	friend class Serializer;
public:
	KeyFrameAnimation(GraphNode* gameObject, std::string&& name = "Animation");
	void setFrame(float time);
	SerializableType getSerializableType() override;
	void update(float msec) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	void renderGui() override;
};
#endif