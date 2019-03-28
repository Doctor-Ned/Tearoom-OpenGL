#include "Render/Shader.h"
#include <fstream>

Shader::Shader(char* vertexPath, char* fragmentPath) : Shader(vertexPath, fragmentPath, true) {}

Shader::Shader(char* vertexPath, char* fragmentPath, bool initialise) {
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

void Shader::remove() {
	glDeleteProgram(id);
}

void Shader::setScale(float scale) {
	use();
	GLint location = getUniformLocation("scale");
	if (location != -1) {
		glUniform1f(location, scale);
	}
}

// currently injected via UboTextureColor, though it might be useful when dealing with non-texture shaders
void Shader::setColor(glm::vec4 color) {
	use();
	GLint location = getUniformLocation("color");
	if (location != -1) {
		glUniform4f(location, color.x, color.y, color.z, color.w);
	}
}

void Shader::setModel(glm::mat4 model) {
	use();
	GLint location = getUniformLocation("model");
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(model));
	}
}

void Shader::setViewPosition(glm::vec3 viewPosition) {
	use();
	GLint location = getUniformLocation("viewPosition");
	if (location != -1) {
		glUniform3f(location, viewPosition.x, viewPosition.y, viewPosition.z);
	}
}

void Shader::setViewDirection(glm::vec3 viewDirection) {
	use();
	GLint location = getUniformLocation("viewDirection");
	if (location != -1) {
		glUniform3f(location, viewDirection.x, viewDirection.y, viewDirection.z);
	}
}

void Shader::setShininess(float shininess) {
	use();
	GLint location = getUniformLocation("shininess");
	if (location != -1) {
		glUniform1f(location, shininess);
	}
}

void Shader::setUseSpecular(bool useSpecular) {
	use();
	GLint location = getUniformLocation("useSpecularMap");
	if (location != -1) {
		glUniform1i(location, useSpecular ? 1 : 0);
	}
}

void Shader::setUseLight(bool useLight) {
	use();
	GLint location = getUniformLocation("useLight");
	if (location != -1) {
		glUniform1i(location, useLight ? 1 : 0);
	}
}

void Shader::setView(glm::mat4 view) {
	use();
	GLint location = getUniformLocation("view");
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(view));
	}
}

void Shader::setProjection(glm::mat4 projection) {
	use();
	GLint location = getUniformLocation("projection");
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(projection));
	}
}

void Shader::setInt(const char* name, int value) {
	use();
	GLint location = getUniformLocation(name);
	if (location != -1) {
		glUniform1i(location, value);
	}
}

void Shader::setFloat(char* name, float value) {
	use();
	GLint location = getUniformLocation(name);
	if(location != -1) {
		glUniform1f(location, value);
	}
}

void Shader::setDirLightSpace(glm::mat4 dirLightSpace) {
	use();
	GLint location = getUniformLocation("dirLightSpace");
	if(location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(dirLightSpace));
	}
}

void Shader::setSpotLightSpace(glm::mat4 spotLightSpace) {
	use();
	GLint location = getUniformLocation("spotLightSpace");
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(spotLightSpace));
	}
}

void Shader::setPointLightSpace(glm::mat4 pointLightSpace) {
	use();
	GLint location = getUniformLocation("pointLightSpace");
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(pointLightSpace));
	}
}

void Shader::setLightSpace(glm::mat4 lightSpace) {
	use();
	GLint location = getUniformLocation("lightSpace");
	if(location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(lightSpace));
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

GLuint Shader::createAndCompileShader(int shaderType, const char* file) {
	GLuint shader;
	shader = glCreateShader(shaderType);
	size_t len_dir = strlen(SHADER_DIR), len_file = strlen(file);
	char* fullFile = new char[len_dir + len_file + 1];
	strcpy(fullFile, SHADER_DIR);
	strcat(fullFile, file);
	std::ifstream data(fullFile, std::ios::binary | std::ios::ate);
	const std::streamsize fileSize = data.tellg();
	data.seekg(0, std::ios::beg);
	char* shaderText = new char[fileSize + 1];
	shaderText[fileSize] = '\0';
	if (data.read(shaderText, fileSize)) {
		delete[] fullFile;
		glShaderSource(shader, 1, (const GLchar**)&shaderText, nullptr);
		glCompileShader(shader);
		GLint isFine, maxLength;
		char* errorLog;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isFine);
		if (isFine == FALSE) {
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
			errorLog = new char[maxLength];
			glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);
			fprintf(stderr, "An error occurred while compiling shader from file '%s'. %s\n", file, errorLog);
			exit(1);
		}
		return shader;
	}
	printf("Unable to read the shader file '%s'.", fullFile);
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
		fprintf(stderr, "An error occurred while linking the shader program. %s\n", errorLog);
		delete errorLog;
		exit(1);
	}
}
