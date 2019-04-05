#include "MeshSimple.h"

MeshSimple::MeshSimple(glm::vec4 color) : Mesh(STColor), color(color) { }

void MeshSimple::draw(Shader *shader, glm::mat4 world) {
	Mesh::draw(shader, world);
	shader->setColor(color);
}

void MeshSimple::setColor(glm::vec4 color) {
	this->color = color;
}
