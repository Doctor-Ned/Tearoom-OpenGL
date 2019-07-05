#pragma once

#include "Ubo/UboViewProjection.h"
#include "Ubo/UboTextureColor.h"
#include "Ubo/UboLights.h"
#include "Render/TextRenderer.h"
#include "GameManager.h"
#include "Global.h"
#include "Components/SkeletalAnimation.h"

struct ModelData;
struct AnimatedModelData;
struct FullModelData;
class Bone;
class Model;

enum ShaderType {
	STNone,
	STSkybox,
	STTexture,
	STColor,
	STModel,
	STModelInstanced,
	STReflect,
	STRefract,
	STDepth,
	STDepthPoint,
	STDepthDebug,
	STUiTexture,
	STUiColor,
	STPostProcessing,
	STLight,
	STBlur,
	STUiText,
	STAnimatedModel,
	STGaussianBlur,
	STText,
	STScreenTexture
};


// Yeah, it looks pretty nice but AFAIK there's no other good way to do this. Let's just make sure these two stay in sync.
static const ShaderType ShaderTypes[] = {
	STNone,
	STSkybox,
	STTexture,
	STColor,
	STModel,
	STModelInstanced,
	STReflect,
	STRefract,
	STDepth,
	STDepthPoint,
	STDepthDebug,
	STUiTexture,
	STUiColor,
	STPostProcessing,
	STLight,
	STBlur,
	STUiText,
	STAnimatedModel,
	STGaussianBlur,
	STText,
	STScreenTexture
};

// haven't had enough lol
static const std::string ShaderTypeNames[] = {
	"STNone",
	"STSkybox",
	"STTexture",
	"STColor",
	"STModel",
	"STModelInstanced",
	"STReflect",
	"STRefract",
	"STDepth",
	"STDepthPoint",
	"STDepthDebug",
	"STUiTexture",
	"STUiColor",
	"STPostProcessing",
	"STLight",
	"STBlur",
	"STUiText",
	"STAnimatedModel",
	"STGaussianBlur",
	"STText",
	"STScreenTexture"
};

enum ResourceType {
	NoneResource,
	TextureResource,
	ModelResource
};


class AssetManager {
public:
	static AssetManager *getInstance();
	GLuint createCubemap(std::vector<std::string> faces);
	TextRenderer* getTextRenderer();
	UboLights* getUboLights();
	UboTextureColor* getUboTextureColor();
	UboViewProjection* getUboViewProjection();
	Shader *getShader(ShaderType type);
	ShaderType getShaderType(Shader *shader);
	std::map<ShaderType, Shader*> getShaders() const;
	Texture getTexture(std::string path);
	Model *getModel(std::string path);
	ModelData* getModelData(std::string path);
	AnimatedModelData* getAnimatedModelData(std::string path);
	Bone *getBoneHierarchy(std::string path);
	SkeletalAnimation getAnimation(std::string path);
	void addBoneHierarchy(std::string path, Bone *boneHierarchy);
	void addAnimation(std::string path, SkeletalAnimation anim);
	static bool endsWith(std::string const& fullString, std::string const& ending);
	AssetManager(AssetManager const&) = delete;
	void operator=(AssetManager const&) = delete;
	void rebindUbos(Shader* shader);
	void refreshShader(Shader* shader);
	void setup();
	~AssetManager();
	bool isLoaded();
	std::vector<std::string> getTextures();
	std::vector<std::string> getModels();
	void reloadResources();
	void loadNextPendingResource();
	Texture getDefaultTexture();
	float getLoadingProgress();
	void addPreloadCallback(const std::string& resource, const std::function<void()>& callback);
private:
	ResourceType getResourceTypeByExtension(const std::string& extension);
	int loadableResources = 1, loadedResources = 0;
	void loadResource(std::string path, bool verify);
	bool loaded = false, loadingStarted=false;
	std::vector<std::string> resourcesToLoad;
	GameManager *gameManager;
	std::vector<Ubo*> ubos;
	Texture defaultTexture;
	Texture createTexture(const char * textureFile);
	std::map <std::string, ResourceType> resourceExtensionMap;
	std::map <std::string, FullModelData*> models;
	std::map <std::string, Bone*> boneHierarchies;
	std::map <std::string, SkeletalAnimation> animations;
	std::map<ShaderType, Shader*> shaders;
	std::map<std::string, std::function<void()>> preloadCallbacks;
	std::vector<Texture> textures;
	AssetManager() {}
	TextRenderer* textRenderer;
	UboLights* uboLights;
	UboTextureColor* uboTextureColor;
	UboViewProjection* uboViewProjection;
};