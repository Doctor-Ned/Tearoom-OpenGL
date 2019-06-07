#ifndef SHADER_H
#define SHADER_H

#include "Global.h"
#include "Ubo/Ubo.h"
#include <map>
#include "LightManager.h"

#define MAX_BONE_TRANSFORMS 80

class Shader {
public:
	Shader(char* vertexPath, char* fragmentPath);
	virtual GLuint getID();
	void use();
	void setColor(glm::vec4 &color);
	void setModel(glm::mat4 &model);
	void setViewPosition(glm::vec3 &viewPosition);
	void setViewDirection(glm::vec3 &viewDirection);
	void setGausBlurAmount(glm::vec3 &blurAmount);
	void setUseLight(bool useLight);
	void setView(glm::mat4 &view);
	void setProjection(glm::mat4 &projection);
	void setLightSpace(glm::mat4 &lightSpace);
	void setCastShadows(bool castShadows);
	void setOpacity(float opacity);
	void setEmissive(glm::vec3 &emissive);
	void setDepthScale(float depthScale);
	void setRoughness(float roughness);
	void setMetallic(float metallic);
	void setAnimatedModelBoneTransforms(glm::mat4 (&boneTransforms)[MAX_BONE_TRANSFORMS]);
	void updateShadowData(std::vector<LightShadowData> dirs, std::vector<LightShadowData> spots, std::vector<LightShadowData> points);
	virtual void bind(Ubo* ubo);
	GLint getUniformLocation(const char* name);
	void setInt(const char* name, int value);
	void setFloat(char* name, float value);
	void setBool(const char* name, bool value);
	void setVec2(const char* name, glm::vec2 &value);
	void setVec3(const char* name, glm::vec3 &value);
	void setVec4(const char* name, glm::vec4 &value);
	void setMat2(const char* name, glm::mat2 &value);
	void setMat3(const char* name, glm::mat3 &value);
	void setMat4(const char* name, glm::mat4 &value);
	virtual ~Shader();
	void setEmissiveFactor(float emissiveFactor);
protected:
	std::map<std::string, std::string> sharedData;
	Shader(char* vertexPath, char* fragmentPath, bool initialise);
	GLuint createAndCompileShader(int shaderType, const char* file);
	virtual void createShaders();
	void deleteShaders();
	void linkShaderProgram();
	GLuint id;
	std::vector<GLuint> shaders;
	char* fragmentPath;
	char* vertexPath;
	const char* SHADER_DIR = "Shaders/";
	const char* SHARED_DIR = "Shaders/Shared";
};

#endif
