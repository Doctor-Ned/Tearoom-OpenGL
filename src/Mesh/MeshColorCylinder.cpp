#include "MeshColorCylinder.h"

MeshColorCylinder::MeshColorCylinder(float radius, float height, int sideAmount, glm::vec4 color,
                                     glm::vec3 baseCenter)
	: MeshSimple(color), baseCenter(baseCenter), height(height), radius(radius), sideAmount(sideAmount) {
	setupMesh();
}

void MeshColorCylinder::draw(Shader *shader, glm::mat4 world) {
	MeshSimple::draw(shader, world);
	glBindVertexArray(VAO);
	glBindVertexBuffer(0, VBO, 0, sizeof(SimpleVertex));
	glDrawArrays(renderMode, 0, vertexAmount);
	glBindVertexArray(0);
	
}

void MeshColorCylinder::updateValues(float radius, float height, int sideAmount) {
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

	std::vector<SimpleVertex> vertices;

	float radStep = 2.0f * M_PI / sideAmount;
	float angle = 0.0f;

	bool last;
	for (int i = 0; i < sideAmount; i++) {
		last = i == sideAmount - 1;
		createBottomTriangle(&vertices, angle, last ? 0.0f : angle + radStep);
		createTopTriangle(&vertices);
		createSideTriangles(&vertices);
		angle += radStep;
	}

	vertexAmount = vertices.size();
	bufferData(&vertices);
	vertices.clear();
}

void MeshColorCylinder::createBottomTriangle(std::vector<SimpleVertex>* vertices, float angle1, float angle2) const {
	SimpleVertex center, closer, farther;
	center.Position = baseCenter;
	center.Normal.x = 0.0f;
	center.Normal.y = -1.0f;
	center.Normal.z = 0.0f;
	farther.Normal = closer.Normal = center.Normal;

	closer.Position.x = baseCenter.x + radius * cos(angle1);
	closer.Position.y = baseCenter.y;
	closer.Position.z = baseCenter.z + radius * sin(angle1);

	farther.Position.x = baseCenter.x + radius * cos(angle2);
	farther.Position.y = baseCenter.y;
	farther.Position.z = baseCenter.z + radius * sin(angle2);

	vertices->push_back(center);
	vertices->push_back(closer);
	vertices->push_back(farther);
}

void MeshColorCylinder::createTopTriangle(std::vector<SimpleVertex>* vertices) const {
	SimpleVertex dummy;
	for (int i = 0; i < 3; i++) {
		vertices->push_back(dummy);
	}
	int index = vertices->size() - 3;
	SimpleVertex *prev, *next;
	for (int i = index - 3; i < index; i++) {
		prev = &(*vertices)[i];
		next = &(*vertices)[i + 3];
		next->Position = prev->Position;
		next->Normal = prev->Normal;
	}
	//std::memcpy(&vertices[index], &vertices[index - 3], sizeof(SimpleVertex) * 3);
	//std::copy(&(*vertices)[index], &(*vertices)[index] + sizeof(SimpleVertex) * 3, std::back_inserter(*vertices));
	//index += 3;
	SimpleVertex* vertex;
	for (int i = index; i < index + 3; i++) {
		vertex = &(*vertices)[i];
		vertex->Position.y += height;
		vertex->Normal.y = 1.0f;
	}
}

void MeshColorCylinder::createSideTriangles(std::vector<SimpleVertex>* vertices) const {
	SimpleVertex dummy;
	for (int i = 0; i < 6; i++) {
		vertices->push_back(dummy);
	}
	int index = vertices->size() - 6;
	SimpleVertex *prev, *next;
	for (int i = index - 6; i < index; i++) {
		prev = &(*vertices)[i];
		next = &(*vertices)[i + 6];
		next->Position = prev->Position;
		next->Normal = prev->Normal;
	}
	//std::memcpy(&vertices[index], &vertices[index - 6], sizeof(SimpleVertex) * 6);
	//std::copy(&(*vertices)[index], &(*vertices)[index] + sizeof(SimpleVertex) * 6, std::back_inserter(*vertices));
	//index += 6;
	SimpleVertex *dwCent = &(*vertices)[index], *dwClos = &(*vertices)[index + 1], *dwFar = &(*vertices)[index + 2],
	             *upCent = &(*vertices)[index + 3], *upFar = &(*vertices)[index + 4], *upClos = &(*vertices)[index + 5];
	SimpleVertex temp; //far and close have reverted names because i need to swap them
	std::memcpy(&temp, upFar, sizeof(SimpleVertex));
	std::memcpy(upFar, upClos, sizeof(SimpleVertex));
	std::memcpy(upClos, &temp, sizeof(SimpleVertex));

	std::memcpy(dwCent, upClos, sizeof(SimpleVertex));
	std::memcpy(upCent, dwFar, sizeof(SimpleVertex));

	glm::vec3 normal;
	normal.y = 0.0f;

	glm::vec2 flatLine(dwFar->Position.x - dwClos->Position.x, dwFar->Position.z - dwClos->Position.z);
	glm::vec2 perpendicular(flatLine.y, -flatLine.x);

	normal.x = perpendicular.x;
	normal.z = perpendicular.y;
	normal = normalize(normal);

	dwCent->Normal = normal;
	dwClos->Normal = normal;
	dwFar->Normal = normal;
	upCent->Normal = normal;
	upClos->Normal = normal;
	upFar->Normal = normal;
}

void MeshColorCylinder::bufferData(std::vector<SimpleVertex>* vertices) {
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

void MeshColorCylinder::setupMesh() {
	glGenVertexArrays(1, &VAO);
	updateValues(radius, height, sideAmount);
}
