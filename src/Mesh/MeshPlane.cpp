#include "MeshPlane.h"

MeshPlane::MeshPlane(float width, float length, char* texturePath, glm::vec3 baseCenter)
	: MeshTexture(), baseCenter(baseCenter), width(width), length(length) {
	texture = AssetManager::getInstance()->getTexture(texturePath);
	this->shader = AssetManager::getInstance()->getShader(getShaderType());
	setupMesh();
}

void MeshPlane::draw(Shader *shader, glm::mat4 world) {
	MeshTexture::draw(shader, world);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glBindVertexArray(VAO);
	glBindVertexBuffer(0, VBO, 0, sizeof(TextureVertex));
	glDrawArrays(renderMode, 0, vertexAmount);
	glBindVertexArray(0);
	glUseProgram(0);
}

void MeshPlane::updateValues(float width, float length) {
	this->width = width;
	this->length = length;

	float halfWidth = width / 2.0f;
	float halfLength = length / 2.0f;

	TextureVertex vertices[4];

	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	vertices[0].Normal = normal; //bl
	vertices[1].Normal = normal; //ul
	vertices[2].Normal = normal; //ur
	vertices[3].Normal = normal; //br

	vertices[0].TexCoords = glm::vec2(0.0f, 1.0f);
	vertices[1].TexCoords = glm::vec2(0.0f, 0.0f);
	vertices[2].TexCoords = glm::vec2(1.0f, 0.0f);
	vertices[3].TexCoords = glm::vec2(1.0f, 1.0f);

	vertices[0].Position = baseCenter;
	vertices[1].Position = baseCenter;
	vertices[2].Position = baseCenter;
	vertices[3].Position = baseCenter;

	vertices[0].Position.x -= halfWidth;
	vertices[0].Position.z += halfLength;

	vertices[1].Position.x -= halfWidth;
	vertices[1].Position.z -= halfLength;

	vertices[2].Position.x += halfWidth;
	vertices[2].Position.z -= halfLength;

	vertices[3].Position.x += halfWidth;
	vertices[3].Position.z += halfLength;

	std::vector<TextureVertex> data;
	data.push_back(vertices[0]);
	data.push_back(vertices[2]);
	data.push_back(vertices[1]);
	data.push_back(vertices[0]);
	data.push_back(vertices[3]);
	data.push_back(vertices[2]);

	vertexAmount = 6;
	shader->use();
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
	}

	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(TextureVertex), &data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), static_cast<void*>(nullptr));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), reinterpret_cast<void*>(offsetof(TextureVertex, Normal)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), reinterpret_cast<void*>(offsetof(TextureVertex, TexCoords)));

	glBindVertexArray(0);
	data.clear();
}

void MeshPlane::setupMesh() {
	glGenVertexArrays(1, &VAO);
	updateValues(width, length);
}
