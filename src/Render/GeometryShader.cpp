#include "GeometryShader.h"

GeometryShader::GeometryShader(char* vertexPath, char* geometryPath, char* fragmentPath) : Shader(vertexPath, fragmentPath, false) {
	this->geometryPath = geometryPath;
	createShaders();
	linkShaderProgram();
	deleteShaders();
}

void GeometryShader::setPointSpaces(glm::mat4 pointSpaces[6]) {
	for (int i = 0; i < 6; i++) {
		setMat4(("pointSpaces[" + std::to_string(i) + "]").c_str(), pointSpaces[i]);
	}
}

void GeometryShader::setPointPosition(glm::vec3 pos) {
	setVec3("lightPos", pos);
}

void GeometryShader::createShaders() {
	shaders.push_back(createAndCompileShader(GL_VERTEX_SHADER, vertexPath));
	shaders.push_back(createAndCompileShader(GL_GEOMETRY_SHADER, geometryPath));
	shaders.push_back(createAndCompileShader(GL_FRAGMENT_SHADER, fragmentPath));
}
