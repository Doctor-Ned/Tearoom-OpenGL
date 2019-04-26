#include "AssetManager.h"
#include "GLFW/glfw3.h"
#include "Render/GeometryShader.h"
#include <filesystem>
#include <fstream>
#include "Scenes/LoadingScene.h"
#include <thread>
#include "Mesh/Model.h"
#include "Render/PostProcessingShader.h"
#include "Render/LightManager.h"

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
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
				data);
			stbi_image_free(data);
		} else {
			printf("Cubemap texture failed to load at path '%s'!\n", faces[i].c_str());
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

Texture AssetManager::createTexture(const char* textureFile) {
	Texture texture;
	int imgWidth, imgHeight, imgChannels;
	unsigned char* imgData = stbi_load(textureFile, &imgWidth, &imgHeight, &imgChannels, 0);
	if (!imgData) {
		fprintf(stderr, "Failed to load texture from file \"%s\"!", textureFile);
		exit(1);
	}
	GLenum format = GL_RGB;
	if (imgChannels == 4) {
		format = GL_RGBA;
	}
	GLuint imgTexture;
	glGenTextures(1, &imgTexture);
	glBindTexture(GL_TEXTURE_2D, imgTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, imgData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imgData);
	texture.id = imgTexture;
	texture.path = textureFile;
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

Shader* AssetManager::getShader(ShaderType type) {
	if (type == STNone) {
		return nullptr;
	}
	return shaders.find(type)->second;
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
	throw "Critial error! An object tried to access an unknown texture: " + path;
}

Model * AssetManager::getModel(std::string path) {
	return new Model(path);
}

std::vector<ModelData*> AssetManager::getModelData(std::string path) {
	for (auto &pair : models) {
		if (pair.first.compare(path) == 0) {
			return pair.second;
		}
	}

	throw "Critical error! An object tried to access an unknown model: " + path;
}

bool AssetManager::endsWith(std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	return false;
}

void AssetManager::setup() {
	shaders.emplace(STText, new Shader("textVS.glsl", "textFS.glsl"));
	textRenderer = new TextRenderer(0.5f);
	textRenderer->load("res/fonts/Merriweather-Regular.ttf", 60);
	shaders.emplace(STUiTexture, new Shader("Ui/uiTextureVS.glsl", "Ui/uiTextureFS.glsl"));
	shaders.emplace(STUiColor, new Shader("Ui/uiColorVS.glsl", "Ui/uiColorFS.glsl"));
	shaders.emplace(STSkybox, new Shader("skyboxVS.glsl", "skyboxFS.glsl"));
	shaders.emplace(STModel, new Shader("modelVS.glsl", "modelFS.glsl"));
	shaders.emplace(STModelInstanced, new Shader("instanceModelVS.glsl", "modelFS.glsl"));
	shaders.emplace(STTexture, new Shader("textureVS.glsl", "textureFS.glsl"));
	shaders.emplace(STColor, new Shader("colorVS.glsl", "colorFS.glsl"));
	shaders.emplace(STReflect, new Shader("Ref/reflectVS.glsl", "Ref/reflectFS.glsl"));
	shaders.emplace(STRefract, new Shader("Ref/refractVS.glsl", "Ref/refractFS.glsl"));
	shaders.emplace(STDepth, new Shader("Depth/depthVS.glsl", "Depth/depthFS.glsl"));
	shaders.emplace(STDepthPoint, new GeometryShader("Depth/depthPointVS.glsl", "Depth/depthPointGS.glsl", "Depth/depthPointFS.glsl"));
	shaders.emplace(STDepthDebug, new Shader("Depth/depthDebugVS.glsl", "Depth/depthDebugFS.glsl"));
	auto ppShader = new PostProcessingShader("Post/postProcessingVS.glsl", "Post/postProcessingFS.glsl");
	shaders.emplace(STPostProcessing, ppShader);
	shaders.emplace(STLight, new Shader("lightVS.glsl", "lightFS.glsl"));
	shaders.emplace(STBlur, new Shader("Post/blurVS.glsl", "Post/blurFS.glsl"));
	LightManager *lightManager = LightManager::getInstance();
	uboLights = new UboLights(0.00f, 0, 0, 0, lightManager->enableLights, lightManager->enableShadowCasting, lightManager->spotDirShadowTexelResolution, lightManager->pointShadowSamples, nullptr, nullptr, nullptr);
	uboTextureColor = new UboTextureColor(false, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	uboViewProjection = new UboViewProjection(glm::mat4(1.0f), glm::mat4(1.0f));
	ubos.push_back(uboLights);
	ubos.push_back(uboTextureColor);
	ubos.push_back(uboViewProjection);
	std::vector<Shader*> shaders;
	shaders.push_back(getShader(STModel));
	shaders.push_back(getShader(STModelInstanced));
	shaders.push_back(getShader(STTexture));
	shaders.push_back(getShader(STColor));

	getShader(STLight)->bind(uboViewProjection);
	getShader(STReflect)->bind(uboViewProjection);
	getShader(STRefract)->bind(uboViewProjection);

	for (int i = 0; i < shaders.size(); i++) {
		for (int j = 0; j < ubos.size(); j++) {
			shaders[i]->bind(ubos[j]);
		}
	}

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
	gameManager = GameManager::getInstance();
	bool verify = loaded;
	for (const auto & entry : fs::recursive_directory_iterator("res")) {
		std::string path = entry.path().u8string();
		for (std::string::iterator i = path.begin(); i != path.end(); ++i) {
			if (*i == '\\') {
				*i = '/';
			}
		}
		bool add = true;
		if (endsWith(path, ".png") || endsWith(path, ".jpg") || endsWith(path, ".tga")) {
			if (verify) {
				for (auto &texture : textures) {
					if (texture.path == path) {
						add = false;
						break;
					}
				}
			}
			if (add) {
				textures.emplace_back(createTexture(path.c_str()));
			}
		} else if (endsWith(path, ".obj")) {
			if (verify) {
				for (auto &pair : models) {
					if (pair.first == path) {
						add = false;
						break;
					}
				}
			}
			if (add) {
				models.emplace(path, Model::createModelData(path));
			}
		}
	}
	loaded = true;
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
