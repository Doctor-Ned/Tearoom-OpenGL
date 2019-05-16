#include "MeshModel.h"

MeshModel::MeshModel(std::vector<ModelVertex> vertices, std::vector<unsigned int> indices, Texture textures[6])
	: Mesh(STModel), indices(indices), vertices(vertices) {
	for (int i = 0; i < 6; i++) {
		this->textures[i] = textures[i];
	}
	setupMesh();
}

void MeshModel::draw(Shader *shader, glm::mat4 world) {
	Mesh::draw(shader, world);
	int counter = 0;
	for(int i=0;i<6;i++) {
		if(textures[i].id != 0) {
			glActiveTexture(GL_TEXTURE0 + counter);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
			shader->setInt(("textures[" + std::to_string(i) + "]").c_str(), counter++);
			shader->setInt(("available[" + std::to_string(i) + "]").c_str(), 1);
		} else {
			shader->setInt(("available[" + std::to_string(i) + "]").c_str(), 0);
		}
	}
	glBindVertexArray(VAO);
	glDrawElements(renderMode, indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	
}

void MeshModel::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ModelVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), static_cast<void*>(nullptr));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), reinterpret_cast<void*>(offsetof(ModelVertex, Normal)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), reinterpret_cast<void*>(offsetof(ModelVertex, TexCoords)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), reinterpret_cast<void*>(offsetof(ModelVertex, Tangent)));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), reinterpret_cast<void*>(offsetof(ModelVertex, Bitangent)));

	glBindVertexArray(0);
}
