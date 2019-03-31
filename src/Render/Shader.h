#ifndef SHADER_H
#define SHADER_H

#include "Global.h"
#include "Ubo/Ubo.h"

class Shader {
public:
	Shader(char* vertexPath, char* fragmentPath);
	virtual GLuint getID();
	void use();
	void setScale(float scale);
	void setColor(glm::vec4 color);
	void setModel(glm::mat4 model);
	void setViewPosition(glm::vec3 viewPosition);
	void setViewDirection(glm::vec3 viewDirection);
	void setShininess(float shininess);
	void setUseSpecular(bool useSpecular);
	void setUseLight(bool useLight);
	void setView(glm::mat4 view);
	void setProjection(glm::mat4 projection);
	void setInt(const char* name, int value);
	void setFloat(char* name, float value);
	void setLightSpace(glm::mat4 lightSpace);
	void updateShadowData(std::vector<LightShadowData> dirs, std::vector<LightShadowData> spots, std::vector<LightShadowData> points);
	virtual void bind(Ubo* ubo);
	GLint getUniformLocation(const char* name);
	virtual ~Shader();
protected:
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
};

#endif
