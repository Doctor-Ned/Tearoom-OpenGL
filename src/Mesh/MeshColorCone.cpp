#include "MeshColorCone.h"

MeshColorCone::MeshColorCone(Shader shader, float radius, float height, int sideAmount, glm::vec4 color,
                             glm::vec3 baseCenter)
	: MeshSimple(shader, color), baseCenter(baseCenter), height(height), radius(radius), sideAmount(sideAmount) {
	VBO = 0;
	setupMesh();
}

void MeshColorCone::draw(Shader shader, glm::mat4 world, float scale) {
	MeshSimple::draw(shader, world, scale);
	glBindVertexArray(VAO);
	glBindVertexBuffer(0, VBO, 0, sizeof(SimpleVertex));
	glDrawArrays(GL_TRIANGLES, 0, vertexAmount);
	glBindVertexArray(0);
	glUseProgram(0);
}

void MeshColorCone::updateValues(float radius, float height, int sideAmount) {
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
		angle += radStep;
	}

	vertexAmount = vertices.size();
	bufferData(&vertices);
	vertices.clear();
}

void MeshColorCone::drawGui(bool autoUpdate) {
	ImGui::PushID((uintptr_t)this);
	static float _radius = radius;
	static float _height = height;
	static int _sideAmount = sideAmount;
	ImGui::SliderFloat("Cone radius", &_radius, 0.01f, 2.0f);
	ImGui::NewLine();
	ImGui::SliderFloat("Cone height", &_height, 0.01f, 2.0f);
	ImGui::NewLine();
	ImGui::SliderInt("Cone sides", &_sideAmount, 3, 50);
	ImGui::NewLine();

	if (autoUpdate || ImGui::Button("Apply cone changes")) {
		if (_radius != radius || _height != height || _sideAmount != sideAmount) {
			radius = _radius;
			height = _height;
			sideAmount = _sideAmount;
			updateValues(radius, height, sideAmount);
		}
	}
	ImGui::PopID();
}

void MeshColorCone::createBottomTriangle(std::vector<SimpleVertex>* vertices, float angle1, float angle2) {
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

void MeshColorCone::createTopTriangle(std::vector<SimpleVertex>* vertices) {
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
	SimpleVertex *cen = &(*vertices)[index], *cl = &(*vertices)[index + 1], *fr = &(*vertices)[index + 2];
	cen->Position.y += height;
	SimpleVertex temp;
	std::memcpy(&temp, cl, sizeof(SimpleVertex));
	std::memcpy(cl, fr, sizeof(SimpleVertex));
	std::memcpy(fr, &temp, sizeof(SimpleVertex));

	glm::vec3 botEdge(cl->Position - fr->Position), sideEdge(cen->Position - fr->Position), normal = normalize(
		          cross(sideEdge, botEdge));

	cen->Normal = fr->Normal = cl->Normal = normal;
}

void MeshColorCone::bufferData(std::vector<SimpleVertex>* vertices) {
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

void MeshColorCone::setupMesh() {
	glGenVertexArrays(1, &VAO);
	updateValues(radius, height, sideAmount);
}
