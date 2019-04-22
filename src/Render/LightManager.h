#pragma once

#define MAX_LIGHTS_OF_TYPE 4    // this MUST be identical to the value from the shader

#include "Global.h"
#include "Serialization/Serializable.h"
#include "Serialization/DataSerializer.h"

class GeometryShader;
class UboLights;
class Shader;

struct DirLight : Serializable {
	DirLight() : ambient(glm::vec4(0.0f)), diffuse(glm::vec4(0.0f)), specular(glm::vec4(0.0f)), model(glm::mat4(1.0f)), enabled(1) {}
	glm::mat4 lightSpace;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::mat4 model;
	glm::vec3 padding;
	int enabled;
	SerializableType getSerializableType() override {
		return SDirLight;
	}
	Json::Value serialize(Serializer* serializer) override {
		Json::Value root;
		root["lightSpace"] = DataSerializer::serializeMat4(lightSpace);
		root["ambient"] = DataSerializer::serializeVec4(ambient);
		root["diffuse"] = DataSerializer::serializeVec4(diffuse);
		root["specular"] = DataSerializer::serializeVec4(specular);
		root["model"] = DataSerializer::serializeMat4(model);
		root["enabled"] = enabled;
		return root;
	}
	void deserialize(Json::Value& root, Serializer* serializer) override {
		lightSpace = DataSerializer::deserializeMat4(root["lightSpace"]);
		ambient = DataSerializer::deserializeVec4(root["ambient"]);
		diffuse = DataSerializer::deserializeVec4(root["diffuse"]);
		specular = DataSerializer::deserializeVec4(root["specular"]);
		model = DataSerializer::deserializeMat4(root["model"]);
		enabled = root["enabled"].asInt();
	}
};

struct PointLight : Serializable {
	PointLight() : constant(100.0f), linear(100.0f), quadratic(100.0f), near_plane(0.01f), far_plane(10.0f),
		ambient(glm::vec4(0.0f)), diffuse(glm::vec4(0.0f)), specular(glm::vec4(0.0f)), model(glm::mat4(1.0f)), enabled(1) {}
	float constant;
	float linear;
	float quadratic;
	float near_plane;
	glm::vec2 padding;
	float far_plane;
	int enabled;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::mat4 model;
	SerializableType getSerializableType() override {
		return SPointLight;
	}
	Json::Value serialize(Serializer* serializer) override {
		Json::Value root;
		root["constant"] = constant;
		root["linear"] = linear;
		root["quadratic"] = quadratic;
		root["near_plane"] = near_plane;
		root["far_plane"] = far_plane;
		root["enabled"] = enabled;
		root["ambient"] = DataSerializer::serializeVec4(ambient);
		root["diffuse"] = DataSerializer::serializeVec4(diffuse);
		root["specular"] = DataSerializer::serializeVec4(specular);
		root["model"] = DataSerializer::serializeMat4(model);
		return root;
	}
	void deserialize(Json::Value& root, Serializer* serializer) override {
		constant = root["constant"].asFloat();
		linear = root["linear"].asFloat();
		quadratic = root["quadratic"].asFloat();
		near_plane = root["near_plane"].asFloat();
		far_plane = root["far_plane"].asFloat();
		enabled = root["enabled"].asInt();
		ambient = DataSerializer::deserializeVec4(root["ambient"]);
		diffuse = DataSerializer::deserializeVec4(root["diffuse"]);
		specular = DataSerializer::deserializeVec4(root["specular"]);
		model = DataSerializer::deserializeMat4(root["model"]);
	}
};

struct SpotLight : Serializable {
	SpotLight() : ambient(glm::vec4(0.0f)), diffuse(glm::vec4(0.0f)), specular(glm::vec4(0.0f)), model(glm::mat4(1.0f)), constant(100.0f), linear(100.0f), quadratic(100.0f),
		cutOff(M_PI / 12.0f), outerCutOff(M_PI / 4.0f), enabled(1) {}
	glm::mat4 lightSpace;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::mat4 model;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	glm::vec2 padding;
	float outerCutOff;
	int enabled;
	SerializableType getSerializableType() override {
		return SSpotLight;
	}
	Json::Value serialize(Serializer* serializer) override {
		Json::Value root;
		root["lightSpace"] = DataSerializer::serializeMat4(lightSpace);
		root["ambient"] = DataSerializer::serializeVec4(ambient);
		root["diffuse"] = DataSerializer::serializeVec4(diffuse);
		root["specular"] = DataSerializer::serializeVec4(specular);
		root["model"] = DataSerializer::serializeMat4(model);
		root["constant"] = constant;
		root["linear"] = linear;
		root["quadratic"] = quadratic;
		root["cutOff"] = cutOff;
		root["outerCutOff"] = outerCutOff;
		root["enabled"] = enabled;
		return root;
	}
	void deserialize(Json::Value& root, Serializer* serializer) override {
		lightSpace = DataSerializer::deserializeMat4(root["lightSpace"]);
		ambient = DataSerializer::deserializeVec4(root["ambient"]);
		diffuse = DataSerializer::deserializeVec4(root["diffuse"]);
		specular = DataSerializer::deserializeVec4(root["specular"]);
		model = DataSerializer::deserializeMat4(root["model"]);
		constant = root["constant"].asFloat();
		linear = root["linear"].asFloat();
		quadratic = root["quadratic"].asFloat();
		cutOff = root["cutOff"].asFloat();
		outerCutOff = root["outerCutOff"].asFloat();
		enabled = root["enabled"].asInt();
	}
};

struct LightShadowData {
	GLuint fbo = 0;
	GLuint rbo = 0;
	GLuint texture = 0;
	GLsizei width = 0;
	GLsizei height = 0;
};

struct DirLightData {
	DirLight *light;
	LightShadowData data;
};

struct SpotLightData {
	SpotLight *light;
	LightShadowData data;
};

struct PointLightData {
	PointLight *light;
	LightShadowData data;
};

struct Lights {
	std::vector<DirLight*> dirLights;
	std::vector<SpotLight*> spotLights;
	std::vector<PointLight*> pointLights;
};

class LightManager {
public:
	static LightManager *getInstance();
	void setup();
	LightManager(LightManager const&) = delete;
	void operator=(LightManager const&) = delete;
	void renderAndUpdate(std::function<void(Shader*)> renderCallback, std::vector<Shader*> updatableShaders);
	float spotNear = 0.4f, spotFar = 25.0f, dirNear = 5.0f, dirFar = 25.0f, dirProjSize = 10.0f;
	Lights getLights();
	Lights recreateLights(int dirs, int spots, int points);
	Lights createUnmanagedLights(int dirs, int spots, int points);
	void replaceLights(Lights lights);
	SpotLight *addSpotLight();
	DirLight *addDirLight();
	PointLight *addPointLight();
	void addSpotLight(SpotLight *light);
	void addDirLight(DirLight *light);
	void addPointLight(PointLight *light);
	void remove(DirLight *light);
	void remove(PointLight *light);
	void remove(SpotLight *light);
	~LightManager();
	int spotDirShadowTexelResolution = 3; // a non-negative power of 3. 
	int pointShadowSamples = 20;          // 20 is nice but we can experiment with some other values.
private:
	LightManager() {}
	void disposeLights();
	void dispose(DirLightData data);
	void dispose(SpotLightData data);
	void dispose(PointLightData data);
	void dispose(LightShadowData data);
	LightShadowData createDirShadowData();
	LightShadowData createSpotShadowData();
	LightShadowData createPointShadowData();
	Shader *depthShader;
	GeometryShader *depthPointShader;
	UboLights *uboLights;
	DirLightData dirLights[MAX_LIGHTS_OF_TYPE];
	SpotLightData spotLights[MAX_LIGHTS_OF_TYPE];
	PointLightData pointLights[MAX_LIGHTS_OF_TYPE];
	int dirLightAmount = 0, spotLightAmount = 0, pointLightAmount = 0;
	float initialAmbient = 0.05f;
	int shadowSize = 2048;
};
