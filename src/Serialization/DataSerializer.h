#pragma once

#include "Global.h"

namespace anim {
	struct ObjectAnimation;
}

struct TransformData;

class DataSerializer {
public:
	static Json::Value serializeVideoSettings(VideoSettings videoSettings);
	static VideoSettings deserializeVideoSettings(const Json::Value &root);
	static Json::Value serializeVec2(glm::vec2 vec);
	static glm::vec2 deserializeVec2(const Json::Value &root);
	static Json::Value serializeVec3(glm::vec3 vec);
	static glm::vec3 deserializeVec3(const Json::Value &root);
	static Json::Value serializeVec4(glm::vec4 vec);
	static glm::vec4 deserializeVec4(const Json::Value &root);
	static Json::Value serializeMat2(glm::mat2 mat);
	static glm::mat2 deserializeMat2(const Json::Value &root);
	static Json::Value serializeMat3(glm::mat3 mat);
	static glm::mat3 deserializeMat3(const Json::Value &root);
	static Json::Value serializeMat4(glm::mat4 mat);
	static glm::mat4 deserializeMat4(const Json::Value &root);
	static Json::Value serializeTransformData(TransformData data);
	static TransformData deserializeTransformData(const Json::Value &root);
	static Json::Value serializeObjectAnimationsMap(std::map<std::string, anim::ObjectAnimation>& map);
	static std::map<std::string, anim::ObjectAnimation> deserializeObjectAnimationsMap(const Json::Value &root);
	static Json::Value serializeObjectAnimation(anim::ObjectAnimation& objAnim);
	static anim::ObjectAnimation deserializeObjectAnimation(const Json::Value &root);
	static Json::Value serializeTransformationMap(std::map<float, glm::vec3>& map);
	static std::map<float, glm::vec3> deserializeTransformationMap(const Json::Value &root);
	
};
