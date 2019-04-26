#pragma once

#include "Global.h"

struct TransformData;

class DataSerializer {
public:
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
};
