#pragma once

#include "Ubo/UboViewProjection.h"
#include "Ubo/UboTextureColor.h"
#include "Ubo/UboLights.h"
#include "Render/TextRenderer.h"
#include "GameManager.h"
#include "Global.h"

struct ModelData;
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
	STText
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
	STText
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
	"STText"
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
	std::map<ShaderType, Shader*> getShaders() const;
	Texture getTexture(std::string path);
	Model *getModel(std::string path);
	ModelData* getModelData(std::string path);
	static bool endsWith(std::string const& fullString, std::string const& ending);
	AssetManager(AssetManager const&) = delete;
	void operator=(AssetManager const&) = delete;
	void setup();
	~AssetManager();
	bool isLoaded();
	std::vector<std::string> getTextures();
	std::vector<std::string> getModels();
	void reloadResources();
	Texture getDefaultTexture();
private:
	bool loaded = false;
	GameManager *gameManager;
	std::vector<Ubo*> ubos;
	Texture defaultTexture;
	Texture createTexture(const char * textureFile);
	std::map <std::string, ModelData*> models;
	std::map<ShaderType, Shader*> shaders;
	std::vector<Texture> textures;
	AssetManager() {}
	TextRenderer* textRenderer;
	UboLights* uboLights;
	UboTextureColor* uboTextureColor;
	UboViewProjection* uboViewProjection;
};