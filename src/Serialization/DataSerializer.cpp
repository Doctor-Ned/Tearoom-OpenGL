#include "DataSerializer.h"

Json::Value DataSerializer::serializeVec2(glm::vec2 vec) {
	Json::Value root;
	root[0] = vec.x;
	root[1] = vec.y;
	return root;
}

glm::vec2 DataSerializer::deserializeVec2(const Json::Value & root) {
	glm::vec2 result;
	result.x = root.get(static_cast<Json::ArrayIndex>(0), 0.0f).asFloat();
	result.y = root.get(static_cast<Json::ArrayIndex>(1), 0.0f).asFloat();
	return result;
}

Json::Value DataSerializer::serializeVec3(glm::vec3 vec) {
	Json::Value root;
	root[0] = vec.x;
	root[1] = vec.y;
	root[2] = vec.z;
	return root;
}

glm::vec3 DataSerializer::deserializeVec3(const Json::Value & root) {
	glm::vec3 result;
	result.x = root.get(static_cast<Json::ArrayIndex>(0), 0.0f).asFloat();
	result.y = root.get(static_cast<Json::ArrayIndex>(1), 0.0f).asFloat();
	result.z = root.get(static_cast<Json::ArrayIndex>(2), 0.0f).asFloat();
	return result;
}

Json::Value DataSerializer::serializeVec4(glm::vec4 vec) {
	Json::Value root;
	root[0] = vec.x;
	root[1] = vec.y;
	root[2] = vec.z;
	root[3] = vec.w;
	return root;
}

glm::vec4 DataSerializer::deserializeVec4(const Json::Value & root) {
	glm::vec4 result;
	result.x = root.get(static_cast<Json::ArrayIndex>(0), 0.0f).asFloat();
	result.y = root.get(static_cast<Json::ArrayIndex>(1), 0.0f).asFloat();
	result.z = root.get(static_cast<Json::ArrayIndex>(2), 0.0f).asFloat();
	result.w = root.get(static_cast<Json::ArrayIndex>(3), 0.0f).asFloat();
	return result;
}

Json::Value DataSerializer::serializeMat2(glm::mat2 mat) {
	Json::Value root;
	root[0] = serializeVec2(mat[0]);
	root[1] = serializeVec2(mat[1]);
	return root;
}

glm::mat2 DataSerializer::deserializeMat2(const Json::Value & root) {
	glm::mat2 result;
	result[0] = deserializeVec2(root[0]);
	result[1] = deserializeVec2(root[1]);
	return result;
}

Json::Value DataSerializer::serializeMat3(glm::mat3 mat) {
	Json::Value root;
	root[0] = serializeVec3(mat[0]);
	root[1] = serializeVec3(mat[1]);
	root[2] = serializeVec3(mat[2]);
	return root;
}

glm::mat3 DataSerializer::deserializeMat3(const Json::Value & root) {
	glm::mat3 result;
	result[0] = deserializeVec3(root[0]);
	result[1] = deserializeVec3(root[1]);
	result[2] = deserializeVec3(root[2]);
	return result;
}

Json::Value DataSerializer::serializeMat4(glm::mat4 mat) {
	Json::Value root;
	root[0] = serializeVec4(mat[0]);
	root[1] = serializeVec4(mat[1]);
	root[2] = serializeVec4(mat[2]);
	root[3] = serializeVec4(mat[3]);
	return root;
}

glm::mat4 DataSerializer::deserializeMat4(const Json::Value & root) {
	glm::mat4 result;
	result[0] = deserializeVec4(root[0]);
	result[1] = deserializeVec4(root[1]);
	result[2] = deserializeVec4(root[2]);
	result[3] = deserializeVec4(root[3]);
	return result;
}
