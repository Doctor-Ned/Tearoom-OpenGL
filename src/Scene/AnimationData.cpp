#include "Scene/AnimationData.h"
#include "Serialization/DataSerializer.h"

Json::Value AnimationData::serialize(Serializer * serializer)
{
	Json::Value root;
	root["objectAnimations"] = DataSerializer::serializeAnimationData(keyFramesForObjects);
	return root;
}

void AnimationData::deserialize(Json::Value & root, Serializer * serializer)
{
	keyFramesForObjects = DataSerializer::deserializeAnimationData(root["objectAnimations"]);
}

SerializableType AnimationData::getSerializableType()
{
	return SAnimationData;
}
