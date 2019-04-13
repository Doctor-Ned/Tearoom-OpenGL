#include "MeshColorTorus.h"

MeshColorTorus::MeshColorTorus(float radiusIn, float radiusOut, int sideAmount, glm::vec4 color,
                               glm::vec3 baseCenter)
	: MeshSimple(color), baseCenter(baseCenter), radiusIn(radiusIn), radiusOut(radiusOut),
	  sideAmount(sideAmount) {
	setupMesh();
}

void MeshColorTorus::draw(Shader *shader, glm::mat4 world) {
	MeshSimple::draw(shader, world);
	glBindVertexArray(VAO);
	glBindVertexBuffer(0, VBO, 0, sizeof(SimpleVertex));
	glDrawArrays(renderMode, 0, vertexAmount);
	glBindVertexArray(0);
	
}

void MeshColorTorus::updateValues(float radiusIn, float radiusOut, int sideAmount) {
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

	std::vector<SimpleVertex> vertices;

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

void MeshColorTorus::createTorusSegment(std::vector<SimpleVertex>* vertices, float angle, float radStep) const {
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

void MeshColorTorus::createRectangle(std::vector<SimpleVertex>* vertices, glm::vec3* tL, glm::vec3* tR, glm::vec3* dR,
                                     glm::vec3* dL) const {
	glm::vec3 horizontal = *dR - *dL;
	glm::vec3 vertical = *tL - *dL;
	glm::vec3 normal = cross(vertical, horizontal);
	SimpleVertex output[4];

	for (int i = 0; i < 4; i++) {
		output[i].Normal = normal;
	}
	output[0].Position = *tL + baseCenter;
	output[1].Position = *tR + baseCenter;
	output[2].Position = *dR + baseCenter;
	output[3].Position = *dL + baseCenter;

	vertices->push_back(output[0]);
	vertices->push_back(output[3]);
	vertices->push_back(output[2]);
	vertices->push_back(output[0]);
	vertices->push_back(output[2]);
	vertices->push_back(output[1]);
}

void MeshColorTorus::bufferData(std::vector<SimpleVertex>* vertices) {
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
	}

	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(SimpleVertex), &(*vertices)[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), static_cast<void*>(nullptr));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), reinterpret_cast<void*>(offsetof(SimpleVertex, Normal)));

	glBindVertexArray(0);
}

void MeshColorTorus::setupMesh() {
	glGenVertexArrays(1, &VAO);
	updateValues(radiusOut, radiusIn, sideAmount);
}
