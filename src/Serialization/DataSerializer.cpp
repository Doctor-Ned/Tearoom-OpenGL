#include "DataSerializer.h"
#include "Scene/ComposedTransform.h"
#include "Scene/Components/Animation.h"


Json::Value DataSerializer::serializeVideoSettings(VideoSettings videoSettings) {
	Json::Value root;
	root["windowWidth"] = videoSettings.windowWidth;
	root["windowHeight"] = videoSettings.windowHeight;
	root["windowType"] = static_cast<int>(videoSettings.windowType);
	return root;
}

VideoSettings DataSerializer::deserializeVideoSettings(const Json::Value& root) {
	VideoSettings result;
	result.windowWidth = root.get("windowWidth", static_cast<float>(UI_REF_WIDTH)).asFloat();
	result.windowHeight = root.get("windowHeight", static_cast<float>(UI_REF_HEIGHT)).asFloat();
	result.windowType = static_cast<WindowType>(root.get("windowType", static_cast<int>(Windowed)).asInt());
	return result;
}

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

Json::Value DataSerializer::serializeTransformData(TransformData data) {
	Json::Value root;
	root["translation"] = serializeVec3(data.translation);
	root["scale"] = serializeVec3(data.scale);
	root["eulerRotation"] = serializeVec3(data.eulerRotation);
	return root;
}

TransformData DataSerializer::deserializeTransformData(const Json::Value& root) {
	TransformData result;
	result.translation = deserializeVec3(root.get("translation", serializeVec3(result.translation)));
	result.scale = deserializeVec3(root.get("scale", serializeVec3(result.scale)));
	result.eulerRotation = deserializeVec3(root.get("eulerRotation", serializeVec3(result.eulerRotation)));
	return result;
}

Json::Value DataSerializer::serializeObjectAnimationsMap(std::map<std::string, anim::ObjectAnimation>& map)
{
	Json::Value root;
	for (auto it = map.begin(); it != map.end(); ++it)
	{
		root[it->first] = serializeObjectAnimation(it->second);
	}
	return root;
}

std::map<std::string, anim::ObjectAnimation> DataSerializer::deserializeObjectAnimationsMap(const Json::Value& root)
{
	std::map<std::string, anim::ObjectAnimation> result;

	for (Json::ValueConstIterator it = root.begin(); it != root.end(); ++it)
	{
		std::string animatedObject = it.key().asString();
		anim::ObjectAnimation anim = deserializeObjectAnimation(root[animatedObject]);
		result.emplace(animatedObject, anim);
	}
	return result;
}

Json::Value DataSerializer::serializeObjectAnimation(anim::ObjectAnimation& objAnim)
{
	Json::Value root;
	root["translation"] = serializeTransformationMap(objAnim.translation);
	root["scale"] = serializeTransformationMap(objAnim.scale);
	root["rotation"] = serializeTransformationMap(objAnim.rotation);
	return root;
}

anim::ObjectAnimation DataSerializer::deserializeObjectAnimation(const Json::Value& root)
{
	anim::ObjectAnimation anim;
	anim.translation = deserializeTransformationMap(root["translation"]);
	anim.scale = deserializeTransformationMap(root["scale"]);
	anim.rotation = deserializeTransformationMap(root["rotation"]);
	return anim;
}

Json::Value DataSerializer::serializeTransformationMap(std::map<float, glm::vec3>& map)
{
	Json::Value root;
	for (auto it : map)
	{
		root[std::to_string(it.first)] = serializeVec3(it.second);
	}
	return root;
}

std::map<float, glm::vec3> DataSerializer::deserializeTransformationMap(const Json::Value& root)
{
	std::map<float, glm::vec3> result;
	for (auto it = root.begin(); it != root.end(); ++it)
	{
		std::string key = it.key().asString();
		glm::vec3 transformation = deserializeVec3(root[key]);
		result.emplace(float(atof(key.c_str())), transformation);
	}
	return result;
}

Json::Value DataSerializer::serializeIntFloatMap(std::map<int, float>& map)
{
	Json::Value root;
	for (auto it : map)
	{
		root[std::to_string(it.first)] = it.second;
	}
	return root;
}

std::map<int, float> DataSerializer::deserializeIntFloatMap(const Json::Value& root)
{
	std::map<int, float> result;
	for (Json::ValueConstIterator it = root.begin(); it != root.end(); ++it)
	{
		float val = root[it.key().asCString()].asFloat();
		std::string key = it.key().asString();
		result.emplace(std::stoi(key), val);
	}
	return result;
}
