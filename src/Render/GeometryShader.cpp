#include "GeometryShader.h"

GeometryShader::GeometryShader(char* vertexPath, char* geometryPath, char* fragmentPath) : Shader(vertexPath, fragmentPath, false) {
	this->geometryPath = geometryPath;
	createShaders();
	linkShaderProgram();
	deleteShaders();
}

void GeometryShader::createShaders() {
	shaders.push_back(createAndCompileShader(GL_VERTEX_SHADER, vertexPath));
	shaders.push_back(createAndCompileShader(GL_GEOMETRY_SHADER, geometryPath));
	shaders.push_back(createAndCompileShader(GL_FRAGMENT_SHADER, fragmentPath));
}
