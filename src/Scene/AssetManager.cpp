#include "AssetManager.h"
#include "Render/GeometryShader.h"
#include <filesystem>
#include "Scenes/LoadingScene.h"
#include <thread>
#include "Mesh/Model.h"
#include "Render/PostProcessingShader.h"
#include "Render/LightManager.h"
#include "Mesh/AnimatedModel.h"
#include "Serialization/Serializer.h"

namespace fs = std::experimental::filesystem;

AssetManager *AssetManager::getInstance() {
	static AssetManager* instance = nullptr;
	if (instance == nullptr) {
		instance = new AssetManager();
	}
	return instance;
}

// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
GLuint AssetManager::createCubemap(std::vector<std::string> faces) {
	GLuint textureID;
	SPDLOG_DEBUG("Creating a new cubemap...");
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load((Global::BASE_PATH + faces[i]).c_str(), &width, &height, &nrComponents, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
						 data);
			stbi_image_free(data);
		} else {
			SPDLOG_ERROR("Cubemap texture failed to load at path '{}'!", faces[i].c_str());
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

Texture AssetManager::createTexture(const char* textureFile) {
	Texture texture;
	int imgWidth, imgHeight, imgChannels;
	unsigned char* imgData = stbi_load(textureFile, &imgWidth, &imgHeight, &imgChannels, 0);
	if (!imgData) {
		SPDLOG_ERROR("Failed to load texture from file \{}\!", textureFile);
		exit(1);
	}
	GLenum format;
	switch (imgChannels) {
		case 1:
			format = GL_RED;
			break;
		case 2:
			format = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			SPDLOG_ERROR("Unexpected channel amount!");
			throw std::exception("Unexpected channel amount!");
	}
	GLuint imgTexture;
	glGenTextures(1, &imgTexture);
	glBindTexture(GL_TEXTURE_2D, imgTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, imgData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imgData);
	texture.id = imgTexture;
	std::string file(textureFile);
	texture.path = file.substr(Global::BASE_PATH.size(), file.size() - Global::BASE_PATH.size());
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

Shader* AssetManager::getShader(ShaderType type) {
	if (type == STNone) {
		return nullptr;
	}
	return shaders.find(type)->second;
}

ShaderType AssetManager::getShaderType(Shader* shader) {
	for(auto &pair : shaders) {
		if(pair.second == shader) {
			return pair.first;
		}
	}
	return STNone;
}

std::map<ShaderType, Shader*> AssetManager::getShaders() const {
	return shaders;
}

Texture AssetManager::getTexture(std::string path) {
	for (auto &texture : textures) {
		if (path == texture.path) {
			return texture;
		}
	}
	SPDLOG_ERROR("An object tried to access an unknown texture: " + path);
	throw "Critial error! An object tried to access an unknown texture: " + path;
}

Model * AssetManager::getModel(std::string path) {
	return new Model(path);
}

ModelData* AssetManager::getModelData(std::string path) {
	for (auto &pair : models) {
		if (pair.first.compare(path) == 0) {
			return &pair.second->simpleData;
		}
	}
	SPDLOG_ERROR("An object tried to access an unknown model: " + path);
	throw "Critical error! An object tried to access an unknown model: " + path;
}

AnimationData* AssetManager::getAnimationData(std::string&& name)
{
	auto it = animationDatasets.find(name);
	if(it->second != nullptr)
	{
		return it->second;
	}
	return nullptr;
}

std::vector<std::string> AssetManager::getAnimationDatasetsNames()
{
	std::vector<std::string> animationDatasetsNames;
	for(auto it : animationDatasets)
	{
		animationDatasetsNames.emplace_back(it.first);
	}
	return animationDatasetsNames;
}

void AssetManager::saveAnimationData(AnimationData* animationData, std::string& name)
{
	Serializer::getInstance()->saveAnimationData(animationData, name);
	animationDatasets[name] = animationData;
}

AnimatedModelData* AssetManager::getAnimatedModelData(std::string path) {
	for (auto &pair : models) {
		if (pair.first.compare(path) == 0) {
			return &pair.second->animatedData;
		}
	}
	SPDLOG_ERROR("An object tried to access an unknown animated model: " + path);
	throw "Critical error! An object tried to access an unknown animated model : " + path;
}

Bone *AssetManager::getBoneHierarchy(std::string path) {
	for (auto &pair : boneHierarchies) {
		if (pair.first.compare(path) == 0) {
			return pair.second;
		}
	}
}

SkeletalAnimation AssetManager::getAnimation(std::string path) {
	for (auto &pair : animations) {
		if (pair.first.compare(path) == 0) {
			return pair.second;
		}
	}
}

void AssetManager::addBoneHierarchy(std::string path, Bone *boneHierarchy) {
	boneHierarchies.emplace(path, boneHierarchy);
}

void AssetManager::addAnimation(std::string path, SkeletalAnimation anim) {
	animations.emplace(path, anim);
}

void AssetManager::updatePPS() {
	PostProcessingShader *pps = dynamic_cast<PostProcessingShader*>(getShader(STPostProcessing));
	pps->use();
	pps->setInt("scene", 0);
	for (int i = 0; i < BLOOM_TEXTURES; i++) {
		pps->setInt(("bloom[" + std::to_string(i) + "]").c_str(), i + 1);
	}
	GameManager* gm = GameManager::getInstance();
	pps->setWindowSize(gm->getWindowWidth(), gm->getWindowHeight());
	pps->setVec2("screenSize", glm::vec2(gm->getWindowWidth(), gm->getWindowHeight()));
}

bool AssetManager::endsWith(std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	return false;
}

void AssetManager::rebindUbos(Shader* shader) {
	for(auto &ubo : ubos) {
		shader->bind(ubo);
	}
}

void AssetManager::refreshShader(Shader* shader) {
	rebindUbos(shader);
	ShaderType type = getShaderType(shader);
	if(type == STModel || type == STModelInstanced || type == STAnimatedModel) {
		shader->use();
		shader->setInt("default_texture", defaultTexture.id);
	}
	if(type == STUiTexture || type == STUiColor || type == STUiText) {
		UiElement::updateProjection();
	}
	if(type == STPostProcessing) {
		updatePPS();
	}
}

void AssetManager::setup() {
	SPDLOG_DEBUG("Setting up AssetManager...");
	shaders.emplace(STUiText, new Shader("Ui/uitextVS.glsl", "Ui/uitextFS.glsl"));
	shaders.emplace(STText, new Shader("textVS.glsl", "textFS.glsl"));
	textRenderer = new TextRenderer(0.5f);
	textRenderer->load("res/fonts/Merriweather-Regular.ttf", 60);
	shaders.emplace(STUiTexture, new Shader("Ui/uiTextureVS.glsl", "Ui/uiTextureFS.glsl"));
	shaders.emplace(STUiColor, new Shader("Ui/uiColorVS.glsl", "Ui/uiColorFS.glsl"));
	shaders.emplace(STSkybox, new Shader("skyboxVS.glsl", "skyboxFS.glsl"));
	shaders.emplace(STModel, new Shader("modelVS.glsl", "modelFS.glsl"));
	shaders.emplace(STAnimatedModel, new Shader("animatedModelVS.glsl", "modelFS.glsl"));
	shaders.emplace(STModelInstanced, new Shader("instanceModelVS.glsl", "modelFS.glsl"));
	shaders.emplace(STTexture, new Shader("textureVS.glsl", "textureFS.glsl"));
	shaders.emplace(STColor, new Shader("colorVS.glsl", "colorFS.glsl"));
	shaders.emplace(STReflect, new Shader("Ref/reflectVS.glsl", "Ref/reflectFS.glsl"));
	shaders.emplace(STRefract, new Shader("Ref/refractVS.glsl", "Ref/refractFS.glsl"));
	shaders.emplace(STGaussianBlur, new Shader("Post/gaussianBlurVS.glsl", "Post/gaussianBlurFS.glsl"));
	shaders.emplace(STDepth, new Shader("Depth/depthVS.glsl", "Depth/depthFS.glsl"));
	shaders.emplace(STDepthPoint, new GeometryShader("Depth/depthPointVS.glsl", "Depth/depthPointGS.glsl", "Depth/depthPointFS.glsl"));
	shaders.emplace(STDepthDebug, new Shader("Depth/depthDebugVS.glsl", "Depth/depthDebugFS.glsl"));
	auto ppShader = new PostProcessingShader("Post/postProcessingVS.glsl", "Post/postProcessingFS.glsl");
	shaders.emplace(STPostProcessing, ppShader);
	shaders.emplace(STLight, new Shader("lightVS.glsl", "lightFS.glsl"));
	shaders.emplace(STBlur, new Shader("Post/blurVS.glsl", "Post/blurFS.glsl"));
	shaders.emplace(STScreenTexture, new Shader("screenTextureVS.glsl", "screenTextureFS.glsl"));
	shaders.emplace(STDownscale, new Shader("Post/downscaleVS.glsl", "Post/downscaleFS.glsl"));
	LightManager *lightManager = LightManager::getInstance();
	uboLights = new UboLights(0.00f, 0, 0, 0, lightManager->enableLights, lightManager->enableShadowCasting, lightManager->lightBleedingReduction, lightManager->minVariance, lightManager->pointShadowSamples, nullptr, nullptr, nullptr);
	uboTextureColor = new UboTextureColor(false, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	uboViewProjection = new UboViewProjection(glm::mat4(1.0f), glm::mat4(1.0f));
	ubos.push_back(uboLights);
	ubos.push_back(uboTextureColor);
	ubos.push_back(uboViewProjection);
	std::vector<Shader*> shaders;
	shaders.push_back(getShader(STModel));
	shaders.push_back(getShader(STAnimatedModel));
	shaders.push_back(getShader(STModelInstanced));
	shaders.push_back(getShader(STTexture));
	shaders.push_back(getShader(STColor));
	shaders.push_back(getShader(STText));

	getShader(STLight)->bind(uboViewProjection);
	getShader(STReflect)->bind(uboViewProjection);
	getShader(STRefract)->bind(uboViewProjection);

	for (auto& shader : shaders) {
		rebindUbos(shader);
	}

	resourceExtensionMap.emplace("fbx", ModelResource);
	resourceExtensionMap.emplace("obj", ModelResource);
	resourceExtensionMap.emplace("jpg", TextureResource);
	resourceExtensionMap.emplace("jpeg", TextureResource);
	resourceExtensionMap.emplace("png", TextureResource);
	resourceExtensionMap.emplace("tga", TextureResource);
	resourceExtensionMap.emplace("anim", AnimationDataResource);
}

AssetManager::~AssetManager() {
	for (auto &ubo : ubos) {
		delete ubo;
	}

	for (auto &pair : shaders) {
		delete pair.second;
	}
	shaders.clear();

	for (auto &texture : textures) {
		glDeleteTextures(1, &texture.id);
	}
}

bool AssetManager::isLoaded() {
	return loaded;
}

std::vector<std::string> AssetManager::getTextures() {
	std::vector<std::string> result;
	for (auto &texture : textures) {
		result.push_back(texture.path);
	}
	return result;
}

std::vector<std::string> AssetManager::getModels() {
	std::vector<std::string> result;
	for (auto &pair : models) {
		result.push_back(pair.first);
	}
	return result;
}

void AssetManager::reloadResources() {
	SPDLOG_DEBUG("Reloading resources...");
	gameManager = GameManager::getInstance();
	for (const auto & entry : fs::recursive_directory_iterator("res")) {
		std::string path = entry.path().u8string();
		for (std::string::iterator i = path.begin(); i != path.end(); ++i) {
			if (*i == '\\') {
				*i = '/';
			}
		}
		loadResource(path, true);
	}
	if (!loaded) {
		defaultTexture = getTexture("res/textures/default.png");
		Shader *shader = getShader(STModel);
		Shader *shader2 = getShader(STModelInstanced);
		Shader *shader3 = getShader(STAnimatedModel);
		shader->use();
		shader->setInt("default_texture", defaultTexture.id);
		shader2->use();
		shader2->setInt("default_texture", defaultTexture.id);
		shader3->use();
		shader3->setInt("default_texture", defaultTexture.id);
		loaded = true;
	}
}

void AssetManager::loadNextPendingResource() {
	if (!loaded) {
		if (!loadingStarted) {
			SPDLOG_DEBUG("Gathering loading data...");
			std::vector<std::string> textures;
			std::vector<std::string> models;
			std::vector<std::string> animationsData = Serializer::getInstance()->getAnimationNames();
			for (const auto & entry : fs::recursive_directory_iterator(Global::BASE_PATH + "res")) {
				std::string path = entry.path().u8string();
				for (std::string::iterator i = path.begin(); i != path.end(); ++i) {
					if (*i == '\\') {
						*i = '/';
					}
				}
				path = path.substr(Global::BASE_PATH.size(), path.size() - Global::BASE_PATH.size());
				switch (getResourceTypeByExtension(Global::getExtension(path))) {
					default:
					case NoneResource:
						continue;
					case TextureResource:
						textures.push_back(path);
						break;
					case ModelResource:
						models.push_back(path);
						break;
				}
			}
			for (auto &str : textures) {
				resourcesToLoad.push_back(str);
			}
			for (auto &str : models) {
				resourcesToLoad.push_back(str);
			}
			for (auto &str : animationsData) {
				resourcesToLoad.push_back(str);
			}
			int index = 0;
			for (auto &pair : preloadCallbacks) {
				bool found = false;
				for (auto i = resourcesToLoad.begin(); i != resourcesToLoad.end();) {
					if (*i == pair.first) {
						found = true;
						resourcesToLoad.erase(i);
						break;
					}
					++i;
				}
				if (!found) {
					SPDLOG_ERROR("Resource '{}' could NOT be preloaded because it doesn't exist!", pair.first.c_str());
					throw std::exception();
				}
				resourcesToLoad.insert(resourcesToLoad.begin() + index, pair.first);
				index++;
			}
			loadableResources = resourcesToLoad.size();
			loadedResources = 0;
			loadingStarted = true;
		} else {
			std::string current = resourcesToLoad.front();
			SPDLOG_DEBUG("Loading '{}'...", current);
			loadResource(current, false);
			auto preload = preloadCallbacks.find(current);
			if(preload != preloadCallbacks.end()) {
				SPDLOG_DEBUG("Running preload callback of '{}'...", current.c_str());
				preload->second();
			}
			resourcesToLoad.erase(resourcesToLoad.begin());
			loadedResources++;
			if (resourcesToLoad.empty()) {
				SPDLOG_DEBUG("Loading finished!");
				loaded = true;
			}
		}
	}
}

Texture AssetManager::getDefaultTexture() {
	return defaultTexture;
}

float AssetManager::getLoadingProgress() {
	return static_cast<float>(loadedResources) / static_cast<float>(loadableResources);
}

void AssetManager::addPreloadCallback(const std::string& resource, const std::function<void()>& callback) {
	for (auto &pair : preloadCallbacks) {
		if (pair.first == resource) {
			SPDLOG_ERROR(("Attempted to add to many preload callbacks to '" + resource + "'!").c_str());
			throw std::exception(("Attempted to add to many preload callbacks to '" + resource + "'!").c_str());
		}
	}
	SPDLOG_DEBUG("Added a preload callback for '{}'.", resource);
	preloadCallbacks.emplace(resource, callback);
}

ResourceType AssetManager::getResourceTypeByExtension(const std::string& extension) {
	if (extension.length() == 0) {
		return NoneResource;
	}
	for (auto &str : resourceExtensionMap) {
		if (str.first == extension) {
			return str.second;
		}
	}
	return NoneResource;
}

void AssetManager::loadResource(std::string path, bool verify) {
	bool add = true;
	switch (getResourceTypeByExtension(Global::getExtension(path))) {
		default:
		case NoneResource:
			break;
		case ModelResource:
			if (verify) {
				for (auto &pair : models) {
					if (pair.first == path) {
						add = false;
						break;
					}
				}
			}
			if (add) {
				models.emplace(path, AnimatedModel::createModelData(Global::BASE_PATH + path));
			}
			break;
		case TextureResource:
			if (verify) {
				for (auto &texture : textures) {
					if (texture.path == path) {
						add = false;
						break;
					}
				}
			}
			if (add) {
				textures.emplace_back(createTexture((Global::BASE_PATH + path).c_str()));
			}
			break;
		case AnimationDataResource:
			if (verify) {
				for (auto &pair : animationDatasets) {
					if (pair.first == path) {
						add = false;
						break;
					}
				}
			}
			if (add) {
				animationDatasets.emplace(path, Serializer::getInstance()->loadAnimationData(path));
			}
			break;
	}
}

TextRenderer* AssetManager::getTextRenderer() {
	return textRenderer;
}

UboLights* AssetManager::getUboLights() {
	return uboLights;
}

UboTextureColor* AssetManager::getUboTextureColor() {
	return uboTextureColor;
}

UboViewProjection* AssetManager::getUboViewProjection() {
	return uboViewProjection;
}
