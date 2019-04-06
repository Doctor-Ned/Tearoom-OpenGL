#pragma once

#define MAX_LIGHTS_OF_TYPE 4    // this MUST be identical to the value from the shader
#include "Scene/DirLightNode.h"

class GeometryShader;
class UboLights;

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
	SpotLight *addSpotLight();
	DirLight *addDirLight();
	PointLight *addPointLight();
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
