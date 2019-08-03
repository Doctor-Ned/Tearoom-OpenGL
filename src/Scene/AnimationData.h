#ifndef ANIMATION_DATA_H
#define ANIMATION_DATA_H
#include <map>
#include <glm/glm.hpp>
#include "Serialization/Serializable.h"

struct KeyFrameData 
{
	std::map<float, glm::vec3> translation;
	std::map<float, glm::vec3> scale;
	std::map<float, glm::vec3> rotation;
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

class AnimationData : public Serializable
{
private:
	
	~AnimationData() = default;
public:
	std::map<std::string, KeyFrameData> keyFramesForObjects;

	AnimationData() = default;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	SerializableType getSerializableType() override;
};
#endif