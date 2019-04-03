#pragma once

#include "Ubo/UboViewProjection.h"
#include "Ubo/UboTextureColor.h"
#include "Ubo/UboLights.h"
#include "Render/TextRenderer.h"
#include "GameManager.h"
#include "Global.h"

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
	STUiColor
};

class AssetManager {
public:
	static AssetManager *getInstance();
	GLuint createCubemap(std::vector<std::string> faces);
	TextRenderer* getTextRenderer();
	UboLights* getUboLights();
	UboTextureColor* getUboTextureColor();
	UboViewProjection* getUboViewProjection();
	Shader * getShader(ShaderType type);
	Texture getTexture(std::string path);
	static bool endsWith(std::string const& fullString, std::string const& ending);
	AssetManager(AssetManager const&) = delete;
	void operator=(AssetManager const&) = delete;
	void setup();
	~AssetManager();
private:
	GameManager *gameManager;
	std::vector<Ubo*> ubos;
	Texture createTexture(const char * textureFile);
	std::map<ShaderType, Shader*> shaders;
	std::vector<Texture> textures;
	AssetManager() {}
	TextRenderer* textRenderer;
	UboLights* uboLights;
	UboTextureColor* uboTextureColor;
	UboViewProjection* uboViewProjection;
};