#include "Mesh.h"
#include "Scene/GameManager.h"

void Mesh::draw(glm::mat4 world) {
	draw(shader, world);
}

void Mesh::setShininess(float shininess) {
	this->shininess = shininess;
}

void Mesh::setUseLight(bool useLight) {
	this->useLight = useLight;
}

bool Mesh::getUseLight() const {
	return useLight;
}

float Mesh::getShininess() const {
	return shininess;
}

void Mesh::draw(Shader *shader, glm::mat4 world) {
	shader->use();
	shader->setShininess(shininess);
	shader->setModel(world);
	shader->setUseLight(useLight);
	shader->setUseSpecular(false);
	shader->setCastShadows(castShadows);
}

GLuint Mesh::getRenderMode() {
	return renderMode;
}
void Mesh::setRenderMode(GLuint renderMode) {
	this->renderMode = renderMode;
}

Mesh::~Mesh() {
	if(VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
	}
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
	}
	if (EBO != 0) {
		glDeleteBuffers(1, &EBO);
	}
}

Mesh::Mesh(GLuint renderMode) : shader(nullptr) {
	this->renderMode = renderMode;
	useLight = GameManager::getInstance()->useLight;
	castShadows = GameManager::getInstance()->castShadows;
}
