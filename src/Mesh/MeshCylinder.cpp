#include "MeshCylinder.h"

MeshCylinder::MeshCylinder(Shader shader, float radius, float height, int sideAmount, char* texturePath,
                           glm::vec3 baseCenter)
	: MeshTexture(shader), baseCenter(baseCenter), height(height), radius(radius), sideAmount(sideAmount) {
	texture = createTexture(texturePath);
	VBO = 0;
	setupMesh();
}

void MeshCylinder::draw(Shader shader, glm::mat4 world, float scale) {
	MeshTexture::draw(shader, world, scale);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glBindVertexArray(VAO);
	glBindVertexBuffer(0, VBO, 0, sizeof(TextureVertex));
	glDrawArrays(GL_TRIANGLES, 0, vertexAmount);
	glBindVertexArray(0);
	glUseProgram(0);
}

void MeshCylinder::updateValues(float radius, float height, int sideAmount) {
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
		createSideTriangles(&vertices);
		angle += radStep;
	}

	vertexAmount = vertices.size();
	bufferData(&vertices);
	vertices.clear();
}

void MeshCylinder::drawGui(bool autoUpdate) {
	ImGui::PushID((uintptr_t)this);
	static float _radius = radius;
	static float _height = height;
	static int _sideAmount = sideAmount;
	ImGui::SliderFloat("Cylinder radius", &_radius, 0.01f, 2.0f);
	ImGui::NewLine();
	ImGui::SliderFloat("Cylinder height", &_height, 0.01f, 2.0f);
	ImGui::NewLine();
	ImGui::SliderInt("Cylinder sides", &_sideAmount, 3, 50);
	ImGui::NewLine();

	if (autoUpdate || ImGui::Button("Apply cylinder changes")) {
		if (_radius != radius || _height != height || _sideAmount != sideAmount) {
			radius = _radius;
			height = _height;
			sideAmount = _sideAmount;
			updateValues(radius, height, sideAmount);
		}
	}
	ImGui::PopID();
}

void MeshCylinder::createBottomTriangle(std::vector<TextureVertex>* vertices, float angle1, float angle2) {
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

void MeshCylinder::createTopTriangle(std::vector<TextureVertex>* vertices) {
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
	//std::memcpy(&vertices[index], &vertices[index - 3], sizeof(TextureVertex) * 3);
	//std::copy(&(*vertices)[index], &(*vertices)[index] + sizeof(TextureVertex) * 3, std::back_inserter(*vertices));
	//index += 3;
	TextureVertex* vertex;
	for (int i = index; i < index + 3; i++) {
		vertex = &(*vertices)[i];
		vertex->Position.y += height;
		vertex->Normal.y = 1.0f;
	}
}

void MeshCylinder::createSideTriangles(std::vector<TextureVertex>* vertices) {
	TextureVertex dummy;
	for (int i = 0; i < 6; i++) {
		vertices->push_back(dummy);
	}
	int index = vertices->size() - 6;
	TextureVertex *prev, *next;
	for (int i = index - 6; i < index; i++) {
		prev = &(*vertices)[i];
		next = &(*vertices)[i + 6];
		next->Position = prev->Position;
		next->Normal = prev->Normal;
		next->TexCoords = prev->TexCoords;
	}
	//std::memcpy(&vertices[index], &vertices[index - 6], sizeof(TextureVertex) * 6);
	//std::copy(&(*vertices)[index], &(*vertices)[index] + sizeof(TextureVertex) * 6, std::back_inserter(*vertices));
	//index += 6;
	TextureVertex *dwCent = &(*vertices)[index], *dwClos = &(*vertices)[index + 1], *dwFar = &(*vertices)[index + 2],
	              *upCent = &(*vertices)[index + 3], *upFar = &(*vertices)[index + 4], *upClos = &(*vertices)[index + 5
	              ];
	TextureVertex temp; //far and close have reverted names because i need to swap them
	std::memcpy(&temp, upFar, sizeof(TextureVertex));
	std::memcpy(upFar, upClos, sizeof(TextureVertex));
	std::memcpy(upClos, &temp, sizeof(TextureVertex));

	std::memcpy(dwCent, upClos, sizeof(TextureVertex));
	std::memcpy(upCent, dwFar, sizeof(TextureVertex));

	dwCent->TexCoords.x = 0.0f;
	dwCent->TexCoords.y = 1.0f;
	upCent->TexCoords.x = 1.0f;
	upCent->TexCoords.y = 0.0f;
	upClos->TexCoords.x = 0.0f;
	upClos->TexCoords.y = 1.0f;
	upFar->TexCoords.x = 1.0f;
	upFar->TexCoords.y = 1.0f;
	dwClos->TexCoords.x = 0.0f;
	dwClos->TexCoords.y = 0.0f;


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

void MeshCylinder::bufferData(std::vector<TextureVertex>* vertices) {
	shader.use();
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

void MeshCylinder::setupMesh() {
	glGenVertexArrays(1, &VAO);
	updateValues(radius, height, sideAmount);
}
