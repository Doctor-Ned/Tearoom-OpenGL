#include "LightManager.h"
#include "GeometryShader.h"
#include "Scene/AssetManager.h"
#include <cstdlib>

LightManager *LightManager::getInstance() {
	static LightManager* instance = nullptr;
	if (instance == nullptr) {
		instance = new LightManager();
	}
	return instance;
}

void LightManager::setup() {
	AssetManager *assetManager = AssetManager::getInstance();
	depthShader = assetManager->getShader(STDepth);
	depthPointShader = dynamic_cast<GeometryShader*>(assetManager->getShader(STDepthPoint));
	uboLights = assetManager->getUboLights();
}

void LightManager::renderAndUpdate(const std::function<void(Shader*)> renderCallback, std::vector<Shader*> updatableShaders) {
	int oldFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);

	depthShader->use();
	for (int i = 0; i < dirLightAmount; i++) {
		DirLightData data = dirLights[i];
		if (!data.light->enabled) {
			continue;
		}

		glViewport(0, 0, data.data.width, data.data.height);
		glBindFramebuffer(GL_FRAMEBUFFER, data.data.fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		glm::vec3 position = glm::vec3(data.light->model[3]);
		glm::mat4 projection = glm::ortho(-dirProjSize, dirProjSize, -dirProjSize, dirProjSize, dirNear, dirFar);
		glm::mat4 directionWorld = data.light->model;
		directionWorld[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		data.light->lightSpace = projection * lookAt(position, position + normalize(glm::vec3(directionWorld * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f))), glm::vec3(0.0f, 1.0f, 0.0f));
		depthShader->setLightSpace(data.light->lightSpace);
		renderCallback(depthShader);
	}

	glm::mat4 spotLightProjection = glm::perspective(glm::radians(45.0f), 1.0f, spotNear, spotFar);

	for (int i = 0; i < spotLightAmount; i++) {
		SpotLightData data = spotLights[i];
		if (!data.light->enabled) {
			continue;
		}

		glViewport(0, 0, data.data.width, data.data.height);
		glBindFramebuffer(GL_FRAMEBUFFER, data.data.fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		glm::mat4 world = data.light->model;
		glm::vec3 pos = world[3];
		glm::mat4 directionWorld = world;
		directionWorld[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		data.light->lightSpace = spotLightProjection * lookAt(pos, pos + normalize(glm::vec3(directionWorld * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f))), glm::vec3(0.0f, 1.0f, 0.0f));
		depthShader->setLightSpace(data.light->lightSpace);
		renderCallback(depthShader);
	}

	depthPointShader->use();
	for (int i = 0; i < pointLightAmount; i++) {
		PointLightData data = pointLights[i];
		if (!data.light->enabled) {
			continue;
		}

		glViewport(0, 0, data.data.width, data.data.height);
		glBindFramebuffer(GL_FRAMEBUFFER, data.data.fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		depthPointShader->setFloat("near_plane", data.light->near_plane);
		depthPointShader->setFloat("far_plane", data.light->far_plane);
		glm::mat4 world = data.light->model;
		glm::vec3 position = world[3];
		depthPointShader->setPointPosition(position);
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, data.light->near_plane, data.light->far_plane);

		glm::mat4 pointSpaces[6];

		static glm::vec3 targets[6] = {
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f)
		};
		static glm::vec3 ups[6] = {
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f)
		};

		for (int i = 0; i < 6; i++) {
			pointSpaces[i] = projection * lookAt(position, position + targets[i], ups[i]);
		}
		depthPointShader->setPointSpaces(pointSpaces);
		renderCallback(depthPointShader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);

	DirLight *dirs = new DirLight[dirLightAmount];
	for (int i = 0; i < dirLightAmount; i++) {
		dirs[i] = *dirLights[i].light;
	}
	SpotLight *spots = new SpotLight[spotLightAmount];
	for (int i = 0; i < spotLightAmount; i++) {
		spots[i] = *spotLights[i].light;
	}
	PointLight *points = new PointLight[pointLightAmount];
	for (int i = 0; i < pointLightAmount; i++) {
		points[i] = *pointLights[i].light;
	}

	uboLights->inject(initialAmbient, dirLightAmount, spotLightAmount, pointLightAmount, spotDirShadowTexelResolution, pointShadowSamples, dirs, spots, points);

	delete[] dirs;
	delete[] spots;
	delete[] points;

	std::vector<LightShadowData> dirData;
	std::vector<LightShadowData> spotData;
	std::vector<LightShadowData> pointData;

	for (int i = 0; i < dirLightAmount; i++) {
		dirData.push_back(dirLights[i].data);
	}

	for (int i = 0; i < spotLightAmount; i++) {
		spotData.push_back(spotLights[i].data);
	}

	for (int i = 0; i < pointLightAmount; i++) {
		pointData.push_back(pointLights[i].data);
	}

	for (auto &shader : updatableShaders) {
		shader->use();
		shader->updateShadowData(dirData, spotData, pointData);
	}
}

Lights LightManager::getLights() {
	Lights result;
	std::vector<DirLight*> dirs;
	std::vector<SpotLight*> spots;
	std::vector<PointLight*> points;
	for (int i = 0; i < dirLightAmount; i++) {
		dirs.push_back(dirLights[i].light);
	}
	for (int i = 0; i < spotLightAmount; i++) {
		spots.push_back(spotLights[i].light);
	}
	for (int i = 0; i < pointLightAmount; i++) {
		points.push_back(pointLights[i].light);
	}
	result.dirLights = dirs;
	result.spotLights = spots;
	result.pointLights = points;
	return result;
}

Lights LightManager::recreateLights(int dirs, int spots, int points) {
	disposeLights();
	if (glm::max(glm::max(dirs, spots), points) > MAX_LIGHTS_OF_TYPE) {
		throw "Attempted to create too many lights!";
	}
	dirLightAmount = dirs;
	spotLightAmount = spots;
	pointLightAmount = points;
	for (int i = 0; i < dirs; i++) {
		dirLights[i].light = new DirLight();
		dirLights[i].data = createDirShadowData();
	}
	for (int i = 0; i < spots; i++) {
		spotLights[i].light = new SpotLight();
		spotLights[i].data = createSpotShadowData();
	}
	for (int i = 0; i < points; i++) {
		pointLights[i].light = new PointLight();
		pointLights[i].data = createPointShadowData();
	}
	return getLights();
}

SpotLight* LightManager::addSpotLight() {
	if (spotLightAmount == MAX_LIGHTS_OF_TYPE) {
		throw "Attempted to add too many spot lights!";
	}
	spotLights[spotLightAmount].light = new SpotLight();
	spotLights[spotLightAmount].data = createSpotShadowData();
	return spotLights[spotLightAmount++].light;
}

DirLight* LightManager::addDirLight() {
	if (dirLightAmount == MAX_LIGHTS_OF_TYPE) {
		throw "Attempted to add too many dir lights!";
	}
	dirLights[dirLightAmount].light = new DirLight();
	dirLights[dirLightAmount].data = createDirShadowData();
	return dirLights[dirLightAmount++].light;
}

PointLight* LightManager::addPointLight() {
	if (pointLightAmount == MAX_LIGHTS_OF_TYPE) {
		throw "Attempted to add too many point lights!";
	}
	pointLights[pointLightAmount].light = new PointLight();
	pointLights[pointLightAmount].data = createPointShadowData();
	return pointLights[pointLightAmount++].light;
}

void LightManager::remove(DirLight * light) {
	int index = -1;
	for (int i = 0; i < dirLightAmount; i++) {
		if (dirLights[i].light == light) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		throw "Attempted to remove a light that is NOT registered in the LightManager!";
	}

	dispose(dirLights[index]);
	for (int i = index + 1; i < dirLightAmount; i++) {
		dirLights[i - 1] = dirLights[i];
	}

	dirLightAmount--;
}

void LightManager::remove(PointLight * light) {
	int index = -1;
	for (int i = 0; i < pointLightAmount; i++) {
		if (pointLights[i].light == light) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		throw "Attempted to remove a light that is NOT registered in the LightManager!";
	}

	dispose(pointLights[index]);
	for (int i = index + 1; i < pointLightAmount; i++) {
		pointLights[i - 1] = pointLights[i];
	}

	pointLightAmount--;
}

void LightManager::remove(SpotLight* light) {
	int index = -1;
	for (int i = 0; i < spotLightAmount; i++) {
		if (spotLights[i].light == light) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		throw "Attempted to remove a light that is NOT registered in the LightManager!";
	}

	dispose(spotLights[index]);
	for (int i = index + 1; i < spotLightAmount; i++) {
		spotLights[i - 1] = spotLights[i];
	}

	spotLightAmount--;
}

LightManager::~LightManager() {
	disposeLights();
}

void LightManager::disposeLights() {
	for (int i = 0; i < dirLightAmount; i++) {
		dispose(dirLights[i]);
	}
	for (int i = 0; i < spotLightAmount; i++) {
		dispose(spotLights[i]);
	}
	for (int i = 0; i < pointLightAmount; i++) {
		dispose(pointLights[i]);
	}
	dirLightAmount = spotLightAmount = pointLightAmount = 0;
}

void LightManager::dispose(DirLightData data) {
	delete data.light;
	dispose(data.data);
}

void LightManager::dispose(SpotLightData data) {
	delete data.light;
	dispose(data.data);
}

void LightManager::dispose(PointLightData data) {
	delete data.light;
	dispose(data.data);
}

void LightManager::dispose(LightShadowData data) {
	glDeleteFramebuffers(1, &data.fbo);
	glDeleteBuffers(1, &data.texture);
}

LightShadowData LightManager::createDirShadowData() {
	LightShadowData result;
	result.width = shadowSize;
	result.height = shadowSize;
	SpecialFramebuffer fb = GameManager::createSpecialFramebuffer(GL_TEXTURE_2D, GL_NEAREST, GL_DEPTH_COMPONENT16, shadowSize, shadowSize, GL_DEPTH_COMPONENT, true, GL_DEPTH_ATTACHMENT);
	result.fbo = fb.fbo;
	result.texture = fb.texture;
	result.rbo = fb.rbo;
	return result;
}

LightShadowData LightManager::createSpotShadowData() {
	return createDirShadowData();
}

LightShadowData LightManager::createPointShadowData() {
	LightShadowData result;
	result.width = shadowSize;
	result.height = shadowSize;
	int oldFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);
	glGenFramebuffers(1, &result.fbo);
	glGenTextures(1, &result.texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, result.texture);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowSize, shadowSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, result.fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, result.texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);
	return result;
}