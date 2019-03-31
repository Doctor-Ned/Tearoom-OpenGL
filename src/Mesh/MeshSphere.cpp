#include "MeshSphere.h"

MeshSphere::MeshSphere(float radius, int precision, char* texturePath, glm::vec3 baseCenter)
	: MeshTexture(), baseCenter(baseCenter), radius(radius), precision(precision) {
	texture = Global::createTexture(texturePath);
	setupMesh();
}

void MeshSphere::draw(Shader *shader, glm::mat4 world, float scale) {
	MeshTexture::draw(shader, world, scale);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glBindVertexArray(VAO);
	glBindVertexBuffer(0, VBO, 0, sizeof(TextureVertex));
	glDrawArrays(GL_TRIANGLES, 0, vertexAmount);
	glBindVertexArray(0);
	glUseProgram(0);
}

void MeshSphere::updateValues(float radius, int precision) {
	if (radius <= 0) {
		radius = 0.01f;
	}
	this->radius = radius;
	if (precision < 3) {
		precision = 3;
	}
	this->precision = precision;

	std::vector<TextureVertex> vertices;

	float radStep = 2.0f * M_PI / precision;
	float angle = 0.0f;

	for (int i = 0; i < precision; i++) {
		createSphereSegment(&vertices, angle, i == precision - 1 ? 2.0f * M_PI - angle : radStep);
		angle += radStep;
	}

	vertexAmount = vertices.size();
	bufferData(&vertices);
	vertices.clear();
}

float MeshSphere::getRadius() const {
	return radius;
}

void MeshSphere::createSphereSegment(std::vector<TextureVertex>* vertices, float angle, float radStep) {
	glm::vec3* circle = new glm::vec3[precision];

	float circleAngle = -M_PI / 2.0f;
	float vertRadStep = M_PI / (precision - 1);

	for (int i = 0; i < precision; i++) {
		if (i == precision - 1) {
			circle[i].x = 0.0f;
			circle[i].y = radius;
		}
		else if (i == 0) {
			circle[i].x = 0.0f;
			circle[i].y = -radius;
		}
		else {
			circle[i].x = radius * cos(circleAngle);
			circle[i].y = radius * sin(circleAngle);
		}
		circle[i].z = 0.0f;

		circleAngle += vertRadStep;
	}

	glm::vec3 rotateAxis(0.0f, 1.0f, 0.0f);

	for (int i = 0; i < precision; i++) {
		circle[i] = rotate(circle[i], angle, rotateAxis);
	}

	glm::vec3* circle2 = new glm::vec3[precision];

	std::memcpy(circle2, circle, precision * sizeof(glm::vec3));

	for (int i = 0; i < precision; i++) {
		circle2[i] = rotate(circle2[i], radStep, rotateAxis);
	}

	for (int i = 0; i < precision - 1; i++) {
		if (i == 0) {
			createTriangle(vertices, &circle[i], &circle[i + 1], &circle2[i + 1]);
		}
		else if (i == precision - 2) {
			createTriangle(vertices, &circle[i + 1], &circle2[i], &circle[i]);
		}
		else {
			createRectangle(vertices, &circle[i], &circle2[i], &circle2[i + 1], &circle[i + 1]);
		}
	}
}

void MeshSphere::createRectangle(std::vector<TextureVertex>* vertices, glm::vec3* tL, glm::vec3* tR, glm::vec3* dR,
                                 glm::vec3* dL) const {
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
	vertices->push_back(output[2]);
	vertices->push_back(output[3]);
	vertices->push_back(output[0]);
	vertices->push_back(output[1]);
	vertices->push_back(output[2]);
}

void MeshSphere::createTriangle(std::vector<TextureVertex>* vertices, glm::vec3* up, glm::vec3* right,
                                glm::vec3* left) {
	glm::vec3 horizontal = *right - *left;
	glm::vec3 vertical = *up - *left;
	glm::vec3 normal = cross(horizontal, vertical);
	TextureVertex output[3];

	for (int i = 0; i < 3; i++) {
		output[i].Normal = normal;
	}
	output[0].Position = *up + baseCenter;
	output[1].Position = *right + baseCenter;
	output[2].Position = *left + baseCenter;

	output[0].TexCoords.x = 0.5f;
	output[0].TexCoords.y = 1.0f;
	output[1].TexCoords.x = 1.0f;
	output[1].TexCoords.y = 0.0f;
	output[2].TexCoords.x = 0.0f;
	output[2].TexCoords.y = 0.0f;

	vertices->push_back(output[0]);
	vertices->push_back(output[2]);
	vertices->push_back(output[1]);
}

void MeshSphere::bufferData(std::vector<TextureVertex>* vertices) {
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

void MeshSphere::setupMesh() {
	glGenVertexArrays(1, &VAO);
	updateValues(radius, precision);
}
