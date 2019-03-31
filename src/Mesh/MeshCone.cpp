#include "MeshCone.h"

MeshCone::MeshCone(float radius, float height, int sideAmount, char* texturePath, glm::vec3 baseCenter)
	: MeshTexture(), baseCenter(baseCenter), height(height), radius(radius), sideAmount(sideAmount) {
	texture = Global::createTexture(texturePath);
	setupMesh();
}

void MeshCone::draw(Shader *shader, glm::mat4 world, float scale) {
	MeshTexture::draw(shader, world, scale);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glBindVertexArray(VAO);
	glBindVertexBuffer(0, VBO, 0, sizeof(TextureVertex));
	glDrawArrays(GL_TRIANGLES, 0, vertexAmount);
	glBindVertexArray(0);
	glUseProgram(0);
}

void MeshCone::updateValues(float radius, float height, int sideAmount) {
	if (radius <= 0) {
		radius = 0.01f;
	}
	this->radius = radius;
	if (height <= 0) {
		height = 0.01f;
	}
	this->height = height;
	if (sideAmount < 3) {
		sideAmount = 3;
	}
	this->sideAmount = sideAmount;

	std::vector<TextureVertex> vertices;

	float radStep = 2.0f * M_PI / sideAmount;
	float angle = 0.0f;

	bool last;
	for (int i = 0; i < sideAmount; i++) {
		last = i == sideAmount - 1;
		createBottomTriangle(&vertices, angle, last ? 0.0f : angle + radStep);
		createTopTriangle(&vertices);
		angle += radStep;
	}

	vertexAmount = vertices.size();
	bufferData(&vertices);
	vertices.clear();
}

void MeshCone::createBottomTriangle(std::vector<TextureVertex>* vertices, float angle1, float angle2) const {
	TextureVertex center, closer, farther;
	center.Position = baseCenter;
	center.Normal.x = 0.0f;
	center.Normal.y = -1.0f;
	center.Normal.z = 0.0f;
	farther.Normal = closer.Normal = center.Normal;

	center.TexCoords.x = 0.5f; //horizontal (U)
	center.TexCoords.y = 1.0f; //vertical (V)

	closer.Position.x = baseCenter.x + radius * cos(angle1);
	closer.Position.y = baseCenter.y;
	closer.Position.z = baseCenter.z + radius * sin(angle1);

	closer.TexCoords.x = 0.0f;
	closer.TexCoords.y = 0.0f;

	farther.Position.x = baseCenter.x + radius * cos(angle2);
	farther.Position.y = baseCenter.y;
	farther.Position.z = baseCenter.z + radius * sin(angle2);

	farther.TexCoords.x = 1.0f;
	farther.TexCoords.y = 0.0f;

	vertices->push_back(center);
	vertices->push_back(closer);
	vertices->push_back(farther);
}

void MeshCone::createTopTriangle(std::vector<TextureVertex>* vertices) const {
	TextureVertex dummy;
	for (int i = 0; i < 3; i++) {
		vertices->push_back(dummy);
	}
	int index = vertices->size() - 3;
	TextureVertex *prev, *next;
	for (int i = index - 3; i < index; i++) {
		prev = &(*vertices)[i];
		next = &(*vertices)[i + 3];
		next->Position = prev->Position;
		next->Normal = prev->Normal;
		next->TexCoords = prev->TexCoords;
	}
	TextureVertex *cen = &(*vertices)[index], *cl = &(*vertices)[index + 1], *fr = &(*vertices)[index + 2];
	cen->Position.y += height;
	TextureVertex temp;
	std::memcpy(&temp, cl, sizeof(TextureVertex));
	std::memcpy(cl, fr, sizeof(TextureVertex));
	std::memcpy(fr, &temp, sizeof(TextureVertex));
	cl->TexCoords.x = 1.0f;
	fr->TexCoords.x = 0.0f;

	glm::vec3 botEdge(cl->Position - fr->Position), sideEdge(cen->Position - fr->Position), normal = normalize(
		          cross(sideEdge, botEdge));

	cen->Normal = fr->Normal = cl->Normal = normal;
}

void MeshCone::bufferData(std::vector<TextureVertex>* vertices) {
	shader->use();
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
	}

	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(TextureVertex), &(*vertices)[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void*)nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void*)offsetof(TextureVertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void*)offsetof(TextureVertex, TexCoords));

	glBindVertexArray(0);
}

void MeshCone::setupMesh() {
	glGenVertexArrays(1, &VAO);
	updateValues(radius, height, sideAmount);
}
