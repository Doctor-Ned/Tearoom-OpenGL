#include "MeshSimple.h"

MeshSimple::MeshSimple(Shader *shader, glm::vec4 color) : Mesh(shader), color(color) {}

void MeshSimple::draw(Shader *shader, glm::mat4 world, float scale) {
	Mesh::draw(shader, world, scale);
	shader->setColor(color);
}

void MeshSimple::setColor(glm::vec4 color) {
	this->color = color;
}
