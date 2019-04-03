#include "MeshModelInstanced.h"

MeshModelInstanced::MeshModelInstanced(std::vector<ModelVertex> vertices,
                                       std::vector<unsigned int> indices, std::vector<ModelTexture> textures,
                                       glm::vec3* offsets, int offsetSize)
	: Mesh(), indices(indices), offsets(offsets), offsetSize(offsetSize), vertices(vertices), textures(textures) {
	this->shader = AssetManager::getInstance()->getShader(getShaderType());
	setupMesh();
}

void MeshModelInstanced::draw(Shader *shader, glm::mat4 world) {
	Mesh::draw(shader, world);
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;
	GLuint heightNr = 1;
	for (GLuint i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);
		else if (name == "texture_height")
			number = std::to_string(heightNr++);

		glUniform1i(shader->getUniformLocation((name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	shader->setUseSpecular(specularNr > 1);
	glBindVertexArray(VAO);
	glDrawElementsInstanced(renderMode, indices.size(), GL_UNSIGNED_INT, nullptr, offsetSize);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glUseProgram(0);
}

ShaderType MeshModelInstanced::getShaderType() {
	return STModelInstanced;
}

void MeshModelInstanced::setupMesh() {
	shader->use();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &instanceVBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * offsetSize, &offsets[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ModelVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, TexCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, Tangent));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, Bitangent));


	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(5, 1);

	glBindVertexArray(0);
}
