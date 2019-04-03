#include "MeshSimple.h"

MeshSimple::MeshSimple(glm::vec4 color) : Mesh(), color(color) {
	this->shader = AssetManager::getInstance()->getShader(getShaderType());
}

void MeshSimple::draw(Shader *shader, glm::mat4 world) {
	Mesh::draw(shader, world);
	shader->setColor(color);
}

ShaderType MeshSimple::getShaderType() {
	return STColor;
}

void MeshSimple::setColor(glm::vec4 color) {
	this->color = color;
}
