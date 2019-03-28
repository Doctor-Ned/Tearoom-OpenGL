#include "MeshSimple.h"

MeshSimple::MeshSimple(Shader shader, std::vector<SimpleVertex> vertices, std::vector<unsigned int> indices)
	: Mesh(shader, indices), vertices(vertices) {
	setupMesh();
}

MeshSimple::MeshSimple(Shader shader, glm::vec4 color) : Mesh(shader), color(color) {}

void MeshSimple::draw(Shader shader, glm::mat4 world, float scale) {
	Mesh::draw(shader, world, scale);
	shader.setColor(color);
}

void MeshSimple::setColor(glm::vec4 color) {
	this->color = color;
}

void MeshSimple::setupMesh() {
	shader.use();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SimpleVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, Normal));

	glBindVertexArray(0);
}
