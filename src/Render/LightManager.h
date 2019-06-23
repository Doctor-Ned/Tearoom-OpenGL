#pragma once

#define MAX_LIGHTS_OF_TYPE 4    // this MUST be identical to the value from the shader

#include "Global.h"
#include "Serialization/Serializable.h"
#include "Serialization/DataSerializer.h"
#include "Scene/GuiConfigurable.h"
#include "Scene/GameManager.h"

struct QuadData;
class GameManager;
class GeometryShader;
class UboLights;
class Shader;

struct DirLight : Serializable {
	DirLight() : color(glm::vec4(1.0f)), model(glm::mat4(1.0f)), enabled(1), strength(1.0f), near_plane(0.01f), far_plane(10.0f), proj_size(10.0f) {}
	glm::mat4 lightSpace;
	glm::vec4 color;
	glm::mat4 model;
	float near_plane;
	float far_plane;
	float strength;
	int enabled;
	glm::vec3 padding;
	float proj_size;
	SerializableType getSerializableType() override {
		return SDirLight;
	}
	Json::Value serialize(Serializer* serializer) override {
		Json::Value root;
		root["lightSpace"] = DataSerializer::serializeMat4(lightSpace);
		root["color"] = DataSerializer::serializeVec4(color);
		root["model"] = DataSerializer::serializeMat4(model);
		root["near_plane"] = near_plane;
		root["far_plane"] = far_plane;
		root["strength"] = strength;
		root["enabled"] = enabled;
		root["proj_size"] = proj_size;
		return root;
	}
	void deserialize(Json::Value& root, Serializer* serializer) override {
		lightSpace = DataSerializer::deserializeMat4(root["lightSpace"]);
		color = DataSerializer::deserializeVec4(root["color"]);
		model = DataSerializer::deserializeMat4(root["model"]);
		near_plane = root.get("near_plane", near_plane).asFloat();
		far_plane = root.get("far_plane", far_plane).asFloat();
		strength = root.get("strength", strength).asFloat();
		enabled = root["enabled"].asInt();
		proj_size = root.get("proj_size", proj_size).asFloat();
	}
};

struct PointLight : Serializable {
	PointLight() : constant(1.0f), linear(1.0f), quadratic(1.0f), near_plane(0.01f), far_plane(10.0f),
		color(glm::vec4(1.0f)), model(glm::mat4(1.0f)), enabled(1) {}
	float constant;
	float linear;
	float quadratic;
	float near_plane;
	glm::vec2 padding;
	float far_plane;
	int enabled;
	glm::vec4 color;
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
		root["color"] = DataSerializer::serializeVec4(color);
		root["model"] = DataSerializer::serializeMat4(model);
		return root;
	}
	void deserialize(Json::Value& root, Serializer* serializer) override {
		constant = root["constant"].asFloat();
		linear = root["linear"].asFloat();
		quadratic = root["quadratic"].asFloat();
		near_plane = root.get("near_plane", near_plane).asFloat();
		far_plane = root.get("far_plane", far_plane).asFloat();
		enabled = root["enabled"].asInt();
		color = DataSerializer::deserializeVec4(root["color"]);
		model = DataSerializer::deserializeMat4(root["model"]);
	}
};

struct SpotLight : Serializable {
	SpotLight() : color(glm::vec4(1.0f)), model(glm::mat4(1.0f)), constant(1.0f), linear(1.0f), quadratic(1.0f),
		cutOff(M_PI / 12.0f), outerCutOff(M_PI / 4.0f), enabled(1), near_plane(0.01f), far_plane(10.0f) {}
	glm::mat4 lightSpace;
	glm::vec4 color;
	glm::mat4 model;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float near_plane;
	float far_plane;
	float outerCutOff;
	int enabled;
	SerializableType getSerializableType() override {
		return SSpotLight;
	}
	Json::Value serialize(Serializer* serializer) override {
		Json::Value root;
		root["lightSpace"] = DataSerializer::serializeMat4(lightSpace);
		root["color"] = DataSerializer::serializeVec4(color);
		root["model"] = DataSerializer::serializeMat4(model);
		root["constant"] = constant;
		root["linear"] = linear;
		root["quadratic"] = quadratic;
		root["near_plane"] = near_plane;
		root["far_plane"] = far_plane;
		root["cutOff"] = cutOff;
		root["outerCutOff"] = outerCutOff;
		root["enabled"] = enabled;
		return root;
	}
	void deserialize(Json::Value& root, Serializer* serializer) override {
		lightSpace = DataSerializer::deserializeMat4(root["lightSpace"]);
		color = DataSerializer::deserializeVec4(root["color"]);
		model = DataSerializer::deserializeMat4(root["model"]);
		constant = root["constant"].asFloat();
		linear = root["linear"].asFloat();
		quadratic = root["quadratic"].asFloat();
		near_plane = root.get("near_plane", near_plane).asFloat();
		far_plane = root.get("far_plane", far_plane).asFloat();
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

struct PreparedLightShadowData {
	LightShadowData data;
	bool prepared = false;
};

struct PreparedFramebuffer {
	Framebuffer framebuffer;
	bool prepared = false;
};

struct Lights {
	std::vector<DirLight*> dirLights;
	std::vector<SpotLight*> spotLights;
	std::vector<PointLight*> pointLights;
	bool operator==(const Lights &l) const {
		if (dirLights.size() != l.dirLights.size()) {
			return false;
		}
		if (spotLights.size() != l.spotLights.size()) {
			return false;
		}
		if (pointLights.size() != l.pointLights.size()) {
			return false;
		}
		for (int i = 0; i < dirLights.size(); i++) {
			if (dirLights[i] != l.dirLights[i]) {
				return false;
			}
		}
		for (int i = 0; i < spotLights.size(); i++) {
			if (dirLights[i] != l.dirLights[i]) {
				return false;
			}
		}
		for (int i = 0; i < pointLights.size(); i++) {
			if (dirLights[i] != l.dirLights[i]) {
				return false;
			}
		}
		return false;
	}
};

enum LightQuality {
	LQLow,
	LQMedium,
	LQHigh
};

static const std::string LightQualities[]{
	"Shadows: Low",
	"Shadows: Medium",
	"Shadows: High"
};

class LightManager : public GuiConfigurable {
public:
	static LightManager *getInstance();
	void setup();
	LightManager(LightManager const&) = delete;
	void operator=(LightManager const&) = delete;
	void renderAndUpdate(std::function<void(Shader*)> renderCallback, std::vector<Shader*> updatableShaders);
	void clearLights();
	bool enableLights = true, enableShadowCasting = true;
	Lights getLights();
	Lights recreateLights(int dirs, int spots, int points);
	Lights createUnmanagedLights(int dirs, int spots, int points);
	void replaceLights(Lights lights);
	SpotLight *addSpotLight();
	DirLight *addDirLight();
	PointLight *addPointLight();
	LightShadowData getLightData(void* light);
	void addSpotLight(SpotLight *light);
	void addDirLight(DirLight *light);
	void addPointLight(PointLight *light);
	void remove(DirLight *light);
	void remove(PointLight *light);
	void remove(SpotLight *light);
	int getDirAmount();
	int getSpotAmount();
	int getPointAmount();
	virtual ~LightManager();
	float lightBleedingReduction = 0.35f;
	float minVariance = 0.00002f;
	int pointShadowSamples = 20;          // 20 is nice but we can experiment with some other values.
	float blurAmount = 1.0f;
	LightQuality getLightQuality();
	void setLightQuality(LightQuality quality);
	static int toShadowSize(LightQuality quality);
private:
	LightManager() {}
	void renderGui() override;
	void disposeLights();
	void dispose(DirLightData data);
	void dispose(SpotLightData data);
	void dispose(PointLightData data);
	void dispose(LightShadowData data);
	QuadData *fullscreenQuad;
	LightShadowData createDirShadowData();
	LightShadowData createSpotShadowData();
	LightShadowData createPointShadowData();
	Framebuffer currentBlurFbo;
	Shader *depthShader, *gausBlurShader;
	GeometryShader *depthPointShader;
	GameManager *gameManager;
	UboLights *uboLights;
	DirLightData dirLights[MAX_LIGHTS_OF_TYPE];
	SpotLightData spotLights[MAX_LIGHTS_OF_TYPE];
	PointLightData pointLights[MAX_LIGHTS_OF_TYPE];
	PreparedLightShadowData dirData[MAX_LIGHTS_OF_TYPE][LQHigh + 1];
	PreparedLightShadowData spotData[MAX_LIGHTS_OF_TYPE][LQHigh + 1];
	PreparedLightShadowData pointData[MAX_LIGHTS_OF_TYPE][LQHigh + 1];
	PreparedFramebuffer blurFbos[LQHigh + 1];
	int dirLightAmount = 0, spotLightAmount = 0, pointLightAmount = 0;
	float initialAmbient = 0.01f;
	LightQuality lightQuality = LQMedium;
};
