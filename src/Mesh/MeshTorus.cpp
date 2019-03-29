#include "MeshTorus.h"

MeshTorus::MeshTorus(Shader *shader, float radiusIn, float radiusOut, int sideAmount, char* texturePath,
                     glm::vec3 baseCenter)
	: MeshTexture(shader), baseCenter(baseCenter), radiusIn(radiusIn), radiusOut(radiusOut), sideAmount(sideAmount) {
	texture = createTexture(texturePath);
	VBO = 0;
	setupMesh();
}

void MeshTorus::draw(Shader *shader, glm::mat4 world, float scale) {
	MeshTexture::draw(shader, world, scale);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glBindVertexArray(VAO);
	glBindVertexBuffer(0, VBO, 0, sizeof(TextureVertex));
	glDrawArrays(GL_TRIANGLES, 0, vertexAmount);
	glBindVertexArray(0);
	glUseProgram(0);
}

void MeshTorus::updateValues(float radiusIn, float radiusOut, int sideAmount) {
	if (radiusIn <= 0) {
		radiusIn = 0.01f;
	}
	this->radiusIn = radiusIn;
	if (radiusOut <= radiusIn) {
		radiusOut = radiusIn + 0.01f;
	}
	this->radiusOut = radiusOut;
	if (sideAmount < 3) {
		sideAmount = 3;
	}
	this->sideAmount = sideAmount;

	std::vector<TextureVertex> vertices;

	float radStep = 2.0f * M_PI / sideAmount;
	float angle = 0.0f;

	for (int i = 0; i < sideAmount; i++) {
		createTorusSegment(&vertices, angle, i == sideAmount - 1 ? 2.0f * M_PI - angle : radStep);
		angle += radStep;
	}

	vertexAmount = vertices.size();
	bufferData(&vertices);
	vertices.clear();
}

void MeshTorus::createTorusSegment(std::vector<TextureVertex>* vertices, float angle, float radStep) {
	glm::vec3* circle = new glm::vec3[sideAmount];

	float centerX = radiusOut;
	float torusRadius = (radiusOut - radiusIn) / 2.0f;
	float circleAngle = 0.0f;

	for (int i = 0; i < sideAmount; i++) {
		circle[i].x = centerX + torusRadius * cos(circleAngle);
		circle[i].y = torusRadius * sin(circleAngle);
		circle[i].z = 0.0f;

		circleAngle += radStep;
	}

	glm::vec3 rotateAxis(0.0f, 1.0f, 0.0f);

	for (int i = 0; i < sideAmount; i++) {
		circle[i] = rotate(circle[i], angle, rotateAxis);
	}

	glm::vec3* circle2 = new glm::vec3[sideAmount];

	std::memcpy(circle2, circle, sideAmount * sizeof(glm::vec3));

	for (int i = 0; i < sideAmount; i++) {
		circle2[i] = rotate(circle2[i], radStep, rotateAxis);
	}

	glm::vec3* tL;
	glm::vec3* tR;
	glm::vec3* dR;
	glm::vec3* dL;

	for (int i = 0; i < sideAmount; i++) {
		tL = &circle[i];
		tR = &circle2[i];
		if (i == sideAmount - 1) {
			dL = &circle[0];
			dR = &circle2[0];
		}
		else {
			dL = &circle[i + 1];
			dR = &circle2[i + 1];
		}
		createRectangle(vertices, tL, tR, dR, dL);

	}
}

void MeshTorus::createRectangle(std::vector<TextureVertex>* vertices, glm::vec3* tL, glm::vec3* tR, glm::vec3* dR,
                                glm::vec3* dL) {
	glm::vec3 horizontal = *dR - *dL;
	glm::vec3 vertical = *tL - *dL;
	glm::vec3 normal = cross(vertical, horizontal);
	TextureVertex output[4];

	for (int i = 0; i < 4; i++) {
		output[i].Normal = normal;
	}
	output[0].Position = *tL + baseCenter;
	output[1].Position = *tR + baseCenter;
	output[2].Position = *dR + baseCenter;
	output[3].Position = *dL + baseCenter;

	output[0].TexCoords.x = 0.0f;
	output[0].TexCoords.y = 1.0f;
	output[1].TexCoords.x = 1.0f;
	output[1].TexCoords.y = 1.0f;
	output[2].TexCoords.x = 1.0f;
	output[2].TexCoords.y = 0.0f;
	output[3].TexCoords.x = 0.0f;
	output[3].TexCoords.y = 0.0f;

	vertices->push_back(output[0]);
	vertices->push_back(output[3]);
	vertices->push_back(output[2]);
	vertices->push_back(output[0]);
	vertices->push_back(output[2]);
	vertices->push_back(output[1]);
}

void MeshTorus::bufferData(std::vector<TextureVertex>* vertices) {
	shader->use();
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
	}

	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(TextureVertex), &(*vertices)[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), static_cast<void*>(nullptr));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), reinterpret_cast<void*>(offsetof(TextureVertex, Normal)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), reinterpret_cast<void*>(offsetof(TextureVertex, TexCoords)));

	glBindVertexArray(0);
}

void MeshTorus::setupMesh() {
	glGenVertexArrays(1, &VAO);
	updateValues(radiusOut, radiusIn, sideAmount);
}
