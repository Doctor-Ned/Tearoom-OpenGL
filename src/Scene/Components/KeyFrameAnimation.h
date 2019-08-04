#ifndef KEYFRAME_ANIMATION_H
#define KEYFRAME_ANIMATION_H
#include "Animation.h"

class KeyFrameAnimation : public Animation
{
private:
	std::vector<GraphNode*> objectsToAnimate;
	inline void takeObjectsToAnimate(GraphNode* objectToAnimate) override;
	inline void interpolateObjects();
	KeyFrameAnimation() = default;
	friend class Serializer;
	void renderGui() override;
public:
	KeyFrameAnimation(GraphNode* gameObject, std::string&& name = "Animation");
	void setFrame(float time);
	SerializableType getSerializableType() override;
	void update(float msec) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
};
#endif