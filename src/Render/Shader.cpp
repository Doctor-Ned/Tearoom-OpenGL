#include "Render/Shader.h"
#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>
#include <streambuf>

namespace fs = std::experimental::filesystem;

Shader::Shader(char* vertexPath, char* fragmentPath) : Shader(vertexPath, fragmentPath, true) {}

Shader::Shader(char* vertexPath, char* fragmentPath, bool initialise) {
	for (const auto & entry : fs::directory_iterator(SHARED_DIR)) {
		std::ifstream t(entry.path());
		if (t) {
			std::string str;
			t.seekg(0, std::ios::end);
			str.reserve(t.tellg());
			t.seekg(0, std::ios::beg);
			str.assign((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());
			sharedData.emplace(entry.path().filename().string(), str);
		} else {
			SPDLOG_ERROR("Cannot read shared shader file '{}'!", entry.path().filename().string().c_str());
			exit(5);
		}
	}

	this->vertexPath = vertexPath;
	this->fragmentPath = fragmentPath;
	id = glCreateProgram();
	if (initialise) {
		createShaders();
		linkShaderProgram();
		deleteShaders();
	}
}

void Shader::createShaders() {
	shaders.push_back(createAndCompileShader(GL_VERTEX_SHADER, vertexPath));
	shaders.push_back(createAndCompileShader(GL_FRAGMENT_SHADER, fragmentPath));
}

void Shader::deleteShaders() {
	while (!shaders.empty()) {
		glDeleteShader(shaders.back());
		shaders.pop_back();
	}
}

GLuint Shader::getID() {
	return id;
}

void Shader::use() {
	glUseProgram(id);
}

void Shader::setColor(glm::vec4 &color) {
	setVec4("color", color);
}

void Shader::setModel(glm::mat4 &model) {
	setMat4("model", model);
}

void Shader::setViewPosition(glm::vec3 &viewPosition) {
	setVec3("viewPosition", viewPosition);
}

void Shader::setViewDirection(glm::vec3 &viewDirection) {
	setVec3("viewDirection", viewDirection);
}

void Shader::setGausBlurAmount(glm::vec3& blurAmount) {
	setVec3("blurScale", blurAmount);
}

void Shader::setUseLight(bool useLight) {
	setBool("useLight", useLight);
}

void Shader::setView(glm::mat4 &view) {
	setMat4("view", view);
}

void Shader::setProjection(glm::mat4 &projection) {
	setMat4("projection", projection);
}

void Shader::setLightSpace(glm::mat4 &lightSpace) {
	setMat4("lightSpace", lightSpace);
}

void Shader::setCastShadows(bool castShadows) {
	setBool("castShadows", castShadows);
}

void Shader::setOpacity(float opacity) {
	setFloat("opacity", opacity);
}

void Shader::setEmissive(glm::vec3& emissive) {
	setVec3("emissive", emissive);
}

void Shader::setRoughness(float roughness) {
	setFloat("roughness", roughness);
}

void Shader::setMetallic(float metallic) {
	setFloat("metallic", metallic);
}

void Shader::setAnimatedModelBoneTransforms(glm::mat4 (&boneTransforms)[MAX_BONE_TRANSFORMS]) {
	for(int i=0;i< MAX_BONE_TRANSFORMS;i++) {
		setMat4(("boneTransforms[" + std::to_string(i) + "]").c_str(), boneTransforms[i]);
	}
}

void Shader::setInt(const char* name, int value) {
	GLint location = getUniformLocation(name);
	if (location != -1) {
		glUniform1i(location, value);
	}
}

void Shader::setFloat(char* name, float value) {
	GLint location = getUniformLocation(name);
	if (location != -1) {
		glUniform1f(location, value);
	}
}

void Shader::setBool(const char * name, bool value) {
	GLint location = getUniformLocation(name);
	if (location != -1) {
		glUniform1i(location, static_cast<int>(value));
	}
}

void Shader::setVec2(const char * name, glm::vec2 & value) {
	GLint location = getUniformLocation(name);
	if (location != -1) {
		glUniform2f(location, value.x, value.y);
	}
}

void Shader::setVec3(const char * name, glm::vec3 & value) {
	GLint location = getUniformLocation(name);
	if (location != -1) {
		glUniform3f(location, value.x, value.y, value.z);
	}
}

void Shader::setVec4(const char * name, glm::vec4 & value) {
	GLint location = getUniformLocation(name);
	if (location != -1) {
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
}

void Shader::setMat2(const char * name, glm::mat2 & value) {
	GLint location = getUniformLocation(name);
	if (location != -1) {
		glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
	}
}

void Shader::setMat3(const char * name, glm::mat3 & value) {
	GLint location = getUniformLocation(name);
	if (location != -1) {
		glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
	}
}

void Shader::setMat4(const char* name, glm::mat4& value) {
	GLint location = getUniformLocation(name);
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}
}

void Shader::updateShadowData(std::vector<LightShadowData> dirs, std::vector<LightShadowData> spots, std::vector<LightShadowData> points) {
	int padding = 0;
	int current;
	const GLuint baseTex = 26;
	for (int i = 0; i < dirs.size(); i++) {
		current = baseTex - padding--;
		glActiveTexture(GL_TEXTURE0 + current);
		glBindTexture(GL_TEXTURE_2D, dirs[i].texture);
		setInt(("dir_shadows[" + std::to_string(i) + "]").c_str(), current);
	}
	for (int i = 0; i < spots.size(); i++) {
		current = baseTex - padding--;
		glActiveTexture(GL_TEXTURE0 +current);
		glBindTexture(GL_TEXTURE_2D, spots[i].texture);
		setInt(("spot_shadows[" + std::to_string(i) + "]").c_str(), current);
	}
	for (int i = 0; i < points.size(); i++) {
		current = baseTex - padding--;
		glActiveTexture(GL_TEXTURE0 + current);
		glBindTexture(GL_TEXTURE_CUBE_MAP, points[i].texture);
		setInt(("point_shadows[" + std::to_string(i) + "]").c_str(), current);
	}
}

void Shader::bind(Ubo* ubo) {
	GLuint index = glGetUniformBlockIndex(id, ubo->getBlockName());
	if (index != -1) {
		glUniformBlockBinding(id, index, ubo->getBinding());
	}
}

GLint Shader::getUniformLocation(const char* name) {
	return glGetUniformLocation(id, name);
}

Shader::~Shader() {
	glDeleteProgram(id);
}

void Shader::setEmissiveFactor(float emissiveFactor) {
	setFloat("emissiveFactor", emissiveFactor);
}

void Shader::setDepthScale(float depthScale)
{
	setFloat("depthScale", depthScale);
}

GLuint Shader::createAndCompileShader(int shaderType, const char* file) {
	GLuint shader;
	shader = glCreateShader(shaderType);
	size_t len_dir = strlen(SHADER_DIR), len_file = strlen(file);
	char* fullFile = new char[len_dir + len_file + 1];
	strcpy(fullFile, SHADER_DIR);
	strcat(fullFile, file);
	std::string text = Global::readFullFile(fullFile);
	if (text.length() > 0) {
		for (std::pair<std::string, std::string> pair : sharedData) {
			size_t start_pos = 0;
			std::string from = "//%" + pair.first + "%";
			while ((start_pos = text.find(from, start_pos)) != std::string::npos) {
				text.replace(start_pos, from.length(), pair.second);
				start_pos += pair.second.length();
			}
		}
		const char* finalText = text.c_str();
		glShaderSource(shader, 1, (const GLchar**)&finalText, nullptr);
		glCompileShader(shader);
		GLint isFine, maxLength;
		char* errorLog;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isFine);
		if (isFine == FALSE) {
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
			errorLog = new char[maxLength];
			glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);
			SPDLOG_ERROR("Unable to compile the shader '{}'! {}", file, errorLog);
			exit(1);
		}
		return shader;
	}
	SPDLOG_ERROR("Unable to read the shader file '{}'!", fullFile);
	delete[] fullFile;
	exit(1);
}

void Shader::linkShaderProgram() {
	for (int i = 0; i < shaders.size(); i++) {
		glAttachShader(id, shaders[i]);
	}
	glLinkProgram(id);
	GLint isFine, maxLength;
	char* errorLog;
	glGetProgramiv(id, GL_LINK_STATUS, (int *)&isFine);
	if (isFine == FALSE) {
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);
		errorLog = new char[maxLength];
		glGetProgramInfoLog(id, maxLength, &maxLength, errorLog);
		SPDLOG_ERROR("Unable to link the shader program! {}", errorLog);
		delete errorLog;
		exit(1);
	}
}
