#include "LightManager.h"
#include "GeometryShader.h"
#include "Scene/AssetManager.h"
#include <cstdlib>
#include "Ui/UiTexturedElement.h"

LightManager *LightManager::getInstance() {
	static LightManager* instance = nullptr;
	if (instance == nullptr) {
		instance = new LightManager();
	}
	return instance;
}

void LightManager::setup() {
	gameManager = GameManager::getInstance();
	AssetManager *assetManager = AssetManager::getInstance();
	depthShader = assetManager->getShader(STDepth);
	gausBlurShader = assetManager->getShader(STGaussianBlur);
	depthPointShader = dynamic_cast<GeometryShader*>(assetManager->getShader(STDepthPoint));
	uboLights = assetManager->getUboLights();
	fullscreenQuad = new QuadData(UiTexturedElement::createFullscreenTexturedQuad());
	blurFbo = GameManager::createFramebuffer(GL_RG32F, SHADOW_SIZE, SHADOW_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, GL_CLAMP_TO_BORDER);
}

void LightManager::renderAndUpdate(const std::function<void(Shader*)> renderCallback, std::vector<Shader*> updatableShaders) {
	if (enableLights) {

		int oldFbo;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);
		glm::vec3 xBlur(blurAmount / SHADOW_SIZE, 0.0f, 0.0f), yBlur(0.0f, blurAmount / SHADOW_SIZE, 0.0f);

		for (int i = 0; i < dirLightAmount; i++) {
			depthShader->use();
			DirLightData data = dirLights[i];
			if (!data.light->enabled) {
				continue;
			}

			glViewport(0, 0, data.data.width, data.data.height);
			glBindFramebuffer(GL_FRAMEBUFFER, data.data.fbo);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glm::vec3 position = glm::vec3(data.light->model[3]);
			glm::mat4 projection = glm::ortho(-data.light->proj_size, data.light->proj_size, -data.light->proj_size, data.light->proj_size, data.light->near_plane, data.light->far_plane);
			glm::mat4 directionWorld = data.light->model;
			directionWorld[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			data.light->lightSpace = projection * lookAt(position, position + normalize(glm::vec3(directionWorld * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f))), glm::vec3(0.0f, 1.0f, 0.0f));
			depthShader->setLightSpace(data.light->lightSpace);
			renderCallback(depthShader);

			gausBlurShader->use();
			glBindFramebuffer(GL_FRAMEBUFFER, blurFbo.fbo);
			glBindTexture(GL_TEXTURE_2D, data.data.texture);
			gausBlurShader->setGausBlurAmount(xBlur);
			fullscreenQuad->render();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data.data.texture, 0);
			glBindTexture(GL_TEXTURE_2D, blurFbo.texture);
			gausBlurShader->setGausBlurAmount(yBlur);
			fullscreenQuad->render();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurFbo.texture, 0);
		}


		for (int i = 0; i < spotLightAmount; i++) {
			depthShader->use();
			SpotLightData data = spotLights[i];
			glm::mat4 spotLightProjection = glm::perspective(glm::radians(45.0f), 1.0f, data.light->near_plane, data.light->far_plane);
			if (!data.light->enabled) {
				continue;
			}

			glViewport(0, 0, data.data.width, data.data.height);
			glBindFramebuffer(GL_FRAMEBUFFER, data.data.fbo);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glm::mat4 world = data.light->model;
			glm::vec3 pos = world[3];
			glm::mat4 directionWorld = world;
			directionWorld[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			data.light->lightSpace = spotLightProjection * lookAt(pos, pos + normalize(glm::vec3(directionWorld * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f))), glm::vec3(0.0f, 1.0f, 0.0f));
			depthShader->setLightSpace(data.light->lightSpace);
			renderCallback(depthShader);

			gausBlurShader->use();
			glBindFramebuffer(GL_FRAMEBUFFER, blurFbo.fbo);
			glBindTexture(GL_TEXTURE_2D, data.data.texture);
			gausBlurShader->setGausBlurAmount(xBlur);
			fullscreenQuad->render();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data.data.texture, 0);
			glBindTexture(GL_TEXTURE_2D, blurFbo.texture);
			gausBlurShader->setGausBlurAmount(yBlur);
			fullscreenQuad->render();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurFbo.texture, 0);
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
			glm::vec3 position = data.light->model[3];
			depthPointShader->setPointPosition(data.light->model[3]);
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
	}

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

	bool noLights = dirLightAmount == 0 && spotLightAmount == 0 && pointLightAmount == 0;
	uboLights->inject(noLights ? 1.0f : initialAmbient, dirLightAmount, spotLightAmount, pointLightAmount, enableLights, enableShadowCasting, spotDirShadowTexelResolution, pointShadowSamples, dirs, spots, points);

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

void LightManager::clearLights() {
	recreateLights(0, 0, 0);
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
	for (int i = 0; i < dirs; i++) {
		addDirLight();
	}
	for (int i = 0; i < spots; i++) {
		addSpotLight();
	}
	for (int i = 0; i < points; i++) {
		addPointLight();
	}
	return getLights();
}

Lights LightManager::createUnmanagedLights(int dirs, int spots, int points) {
	Lights lights;
	for (int i = 0; i < dirs; i++) {
		lights.dirLights.push_back(new DirLight());
	}
	for (int i = 0; i < spots; i++) {
		lights.spotLights.push_back(new SpotLight());
	}
	for (int i = 0; i < points; i++) {
		lights.pointLights.push_back(new PointLight());
	}
	return lights;
}

void LightManager::replaceLights(Lights lights) {
	disposeLights();
	if (std::max(lights.pointLights.size(), std::max(lights.spotLights.size(), lights.dirLights.size())) > MAX_LIGHTS_OF_TYPE) {
		throw "Attempted to add too many lights!";
	}
	for (auto dir : lights.dirLights) {
		addDirLight(dir);
	}
	for (auto spot : lights.spotLights) {
		addSpotLight(spot);
	}
	for (auto point : lights.pointLights) {
		addPointLight(point);
	}
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

LightShadowData LightManager::getLightData(void* light) {
	for (int i = 0; i < dirLightAmount; i++) {
		if (static_cast<void*>(dirLights[i].light) == light) {
			return dirLights[i].data;
		}
	}
	for (int i = 0; i < spotLightAmount; i++) {
		if (static_cast<void*>(spotLights[i].light) == light) {
			return spotLights[i].data;
		}
	}
	for (int i = 0; i < pointLightAmount; i++) {
		if (static_cast<void*>(pointLights[i].light) == light) {
			return pointLights[i].data;
		}
	}
	return LightShadowData();
}

void LightManager::addSpotLight(SpotLight* light) {
	for (int i = 0; i < spotLightAmount; i++) {
		if (spotLights[i].light == light) {
			return;
		}
	}
	if (spotLightAmount == MAX_LIGHTS_OF_TYPE) {
		throw "Attempted to add too many spot lights!";
	}
	spotLights[spotLightAmount].light = light;
	spotLights[spotLightAmount].data = createSpotShadowData();
	spotLightAmount++;
}

void LightManager::addDirLight(DirLight* light) {
	for (int i = 0; i < dirLightAmount; i++) {
		if (dirLights[i].light == light) {
			return;
		}
	}
	if (dirLightAmount == MAX_LIGHTS_OF_TYPE) {
		throw "Attempted to add too many dir lights!";
	}
	dirLights[dirLightAmount].light = light;
	dirLights[dirLightAmount].data = createDirShadowData();
	dirLightAmount++;
}

void LightManager::addPointLight(PointLight* light) {
	for (int i = 0; i < pointLightAmount; i++) {
		if (pointLights[i].light == light) {
			return;
		}
	}
	if (pointLightAmount == MAX_LIGHTS_OF_TYPE) {
		throw "Attempted to add too many point lights!";
	}
	pointLights[pointLightAmount].light = light;
	pointLights[pointLightAmount].data = createPointShadowData();
	pointLightAmount++;
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

int LightManager::getDirAmount() {
	return dirLightAmount;
}

int LightManager::getSpotAmount() {
	return spotLightAmount;
}

int LightManager::getPointAmount() {
	return pointLightAmount;
}

LightManager::~LightManager() {
	disposeLights();
	fullscreenQuad->dispose();
	delete fullscreenQuad;
	glDeleteBuffers(1, &blurFbo.texture);
	glDeleteFramebuffers(1, &blurFbo.fbo);
}

void LightManager::renderGui() {
	ImGui::DragFloat("Initial ambient", &initialAmbient, 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat("Blur amount", &blurAmount, 0.01f, 0.0f, 10.0f);
	std::string dirText, spotText, pointText;
	dirText = std::to_string(dirLightAmount) + " directional lights";
	spotText = std::to_string(spotLightAmount) + " spot lights";
	pointText = std::to_string(pointLightAmount) + " point lights";

	ImGui::Text(dirText.c_str());
	ImGui::Text(spotText.c_str());
	ImGui::Text(pointText.c_str());
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
	result.width = SHADOW_SIZE;
	result.height = SHADOW_SIZE;
	SpecialFramebuffer fb = GameManager::createSpecialFramebuffer(GL_TEXTURE_2D, GL_LINEAR, GL_RG32F, SHADOW_SIZE, SHADOW_SIZE, GL_RGBA, true, GL_COLOR_ATTACHMENT0);
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
	result.width = SHADOW_SIZE;
	result.height = SHADOW_SIZE;
	int oldFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);
	glGenFramebuffers(1, &result.fbo);
	glGenTextures(1, &result.texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, result.texture);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, result.fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, result.texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);
	return result;
}