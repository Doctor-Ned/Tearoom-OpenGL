#include "MeshRefSphere.h"
#include "MeshTexture.h"

MeshRefSphere::MeshRefSphere(Shader shader, float radius, int precision, glm::vec3 baseCenter)
	: MeshRef(shader), baseCenter(baseCenter), radius(radius), precision(precision) {
	VBO = 0;
	setupMesh();
}

void MeshRefSphere::draw(Shader shader, glm::mat4 world, float scale) {
	MeshRef::draw(shader, world, scale);
	//glBindVertexArray(VAO);
	glBindVertexBuffer(0, VBO, 0, sizeof(SimpleVertex));
	glDrawArrays(GL_TRIANGLES, 0, vertexAmount);
	glBindVertexArray(0);
	glUseProgram(0);
}

void MeshRefSphere::updateValues(float radius, int precision) {
	if (radius <= 0) {
		radius = 0.01f;
	}
	this->radius = radius;
	if (precision < 3) {
		precision = 3;
	}
	this->precision = precision;

	std::vector<SimpleVertex> vertices;

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

void MeshRefSphere::drawGui(bool autoUpdate) {
	ImGui::PushID((uintptr_t)this);
	static float _radius = radius;
	static int _precision = precision;
	ImGui::SliderFloat("Sphere radius", &_radius, 0.01f, 2.0f);
	ImGui::NewLine();
	ImGui::SliderInt("Sphere precision", &_precision, 3, 50);
	ImGui::NewLine();

	if (autoUpdate || ImGui::Button("Apply sphere changes")) {
		if (_radius != radius || _precision != precision) {
			radius = _radius;
			precision = _precision;
			updateValues(radius, precision);
		}
	}
	ImGui::PopID();
}

float MeshRefSphere::getRadius() {
	return radius;
}

glm::vec3 MeshRefSphere::getUnmodeledCenter() {
	return baseCenter;
}

void MeshRefSphere::createSphereSegment(std::vector<SimpleVertex>* vertices, float angle, float radStep) {
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

void MeshRefSphere::createRectangle(std::vector<SimpleVertex>* vertices, glm::vec3* tL, glm::vec3* tR, glm::vec3* dR,
                                 glm::vec3* dL) {
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

void MeshRefSphere::createTriangle(std::vector<SimpleVertex>* vertices, glm::vec3* up, glm::vec3* right,
                                glm::vec3* left) {
	glm::vec3 horizontal = *right - *left;
	glm::vec3 vertical = *up - *left;
	glm::vec3 normal = cross(horizontal, vertical);
	SimpleVertex output[3];

	for (int i = 0; i < 3; i++) {
		output[i].Normal = normal;
	}
	output[0].Position = *up + baseCenter;
	output[1].Position = *right + baseCenter;
	output[2].Position = *left + baseCenter;

	vertices->push_back(output[0]);
	vertices->push_back(output[2]);
	vertices->push_back(output[1]);
}

void MeshRefSphere::bufferData(std::vector<SimpleVertex>* vertices) {
	shader.use();
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
	}

	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(SimpleVertex), &(*vertices)[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, Normal));

	glBindVertexArray(0);
}

void MeshRefSphere::setupMesh() {
	glGenVertexArrays(1, &VAO);
	updateValues(radius, precision);
}
