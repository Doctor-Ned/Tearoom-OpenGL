#include "GeometryShader.h"

GeometryShader::GeometryShader(char* vertexPath, char* geometryPath, char* fragmentPath) : Shader(vertexPath, fragmentPath, false) {
	this->geometryPath = geometryPath;
	createShaders();
	linkShaderProgram();
	deleteShaders();
}

void GeometryShader::setPointSpaces(glm::mat4 pointSpaces[6]) {
	use();
	for (int i = 0; i < 6; i++) {
		GLuint location = getUniformLocation(("pointSpaces[" + std::to_string(i) + "]").c_str());
		if (location != -1) {
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(pointSpaces[i]));
		}
	}
}

void GeometryShader::setPointPosition(glm::vec3 pos) {
	use();
	GLuint location = getUniformLocation("lightPos");
	if (location != -1) {
		glUniform3f(location, pos.x, pos.y, pos.z);
	}
}

void GeometryShader::createShaders() {
	shaders.push_back(createAndCompileShader(GL_VERTEX_SHADER, vertexPath));
	shaders.push_back(createAndCompileShader(GL_GEOMETRY_SHADER, geometryPath));
	shaders.push_back(createAndCompileShader(GL_FRAGMENT_SHADER, fragmentPath));
}
