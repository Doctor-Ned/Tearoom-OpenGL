#include "MeshCylinder.h"
#include "Serialization/DataSerializer.h"

MeshCylinder::MeshCylinder(float radius, float height, int sideAmount, std::string texturePath,
                           glm::vec3 baseCenter)
	: MeshTexture(), baseCenter(baseCenter), height(height), radius(radius), sideAmount(sideAmount) {
	texture = AssetManager::getInstance()->getTexture(texturePath);
	setupMesh();
}

void MeshCylinder::renderGui() {
	MeshTexture::renderGui();
	float radius = this->radius, height = this->height;
	int sideAmount = this->sideAmount;
	glm::vec3 baseCenter = this->baseCenter;
	ImGui::DragFloat3("Base center", reinterpret_cast<float*>(&baseCenter), 0.1f);
	ImGui::InputFloat3("Base center (fixed)", reinterpret_cast<float*>(&baseCenter));
	ImGui::DragInt("Side amount", &sideAmount, 1, 3, std::numeric_limits<int>::max());
	ImGui::SameLine();
	ImGui::InputInt("Side amount (fixed)", &sideAmount);
	if (sideAmount < 3) {
		sideAmount = 3;
	}
	ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, std::numeric_limits<float>::max());
	ImGui::SameLine();
	ImGui::InputFloat("Radius (fixed)", &radius);
	ImGui::DragFloat("Height", &height, 0.1f, 0.0f, std::numeric_limits<float>::max());
	ImGui::SameLine();
	ImGui::InputFloat("Height (fixed)", &height);
	if (radius < 0.0f) {
		radius = 0.0f;
	}
	if (height < 0.0f) {
		height = 0.0f;
	}
	if (baseCenter != this->baseCenter || sideAmount != this->sideAmount || radius != this->radius || height != this->height) {
		updateValues(radius, height, sideAmount, baseCenter);
	}
}

void MeshCylinder::draw(Shader *shader, glm::mat4 world) {
	MeshTexture::draw(shader, world);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glBindVertexArray(VAO);
	glBindVertexBuffer(0, VBO, 0, sizeof(TextureVertex));
	glDrawArrays(renderMode, 0, vertexAmount);
	glBindVertexArray(0);
	
}

void MeshCylinder::updateValues(float radius, float height, int sideAmount) {
	updateValues(radius, height, sideAmount, baseCenter);
}

void MeshCylinder::updateValues(float radius, float height, int sideAmount, glm::vec3 baseCenter) {
	this->baseCenter = baseCenter;
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

SerializableType MeshCylinder::getSerializableType() {
	return SMeshCylinder;
}

Json::Value MeshCylinder::serialize(Serializer* serializer) {
	Json::Value root = MeshTexture::serialize(serializer);
	root["baseCenter"] = DataSerializer::serializeVec3(baseCenter);
	root["radius"] = radius;
	root["height"] = height;
	root["sideAmount"] = sideAmount;
	return root;
}

void MeshCylinder::deserialize(Json::Value& root, Serializer* serializer) {
	MeshTexture::deserialize(root, serializer);
	baseCenter = DataSerializer::deserializeVec3(root.get("baseCenter", DataSerializer::serializeVec3(glm::vec3(0.0f, 0.0f, 0.0f))));
	radius = root["radius"].asFloat();
	height = root["height"].asFloat();
	sideAmount = root["sideAmount"].asInt();
	setupMesh();
}

void MeshCylinder::createBottomTriangle(std::vector<TextureVertex>* vertices, float angle1, float angle2) const {
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

void MeshCylinder::createTopTriangle(std::vector<TextureVertex>* vertices) const {
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
	TextureVertex v = (*vertices)[index + 1];
	(*vertices)[index + 1] = (*vertices)[index + 2];
	(*vertices)[index + 2] = v;
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

void MeshCylinder::createSideTriangles(std::vector<TextureVertex>* vertices) const {
	int index = vertices->size() - 6;
	TextureVertex dwLeft = (*vertices)[index+2], dwRight= (*vertices)[index+1], upLeft=(*vertices)[index+4], upRight=(*vertices)[index+5];

	dwLeft.TexCoords = glm::vec2(0.0f, 1.0f);
	dwRight.TexCoords = glm::vec2(1.0f, 1.0f);
	upLeft.TexCoords = glm::vec2(0.0f, 0.0f);
	upRight.TexCoords = glm::vec2(1.0f, 0.0f);

	glm::vec3 normal;
	normal.y = 0.0f;

	glm::vec2 flatLine(dwLeft.Position.x - dwRight.Position.x, dwLeft.Position.z - dwRight.Position.z);
	glm::vec2 perpendicular(flatLine.y, -flatLine.x);

	normal.x = perpendicular.x;
	normal.z = perpendicular.y;
	normal = normalize(normal);
	
	dwLeft.Normal = normal;
	dwRight.Normal = normal;
	upLeft.Normal = normal;
	upRight.Normal = normal;

	vertices->push_back(dwLeft);
	vertices->push_back(upRight);
	vertices->push_back(upLeft);
	vertices->push_back(dwLeft);
	vertices->push_back(dwRight);
	vertices->push_back(upRight);
}

void MeshCylinder::bufferData(std::vector<TextureVertex>* vertices) {
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
	updateValues(radius, height, sideAmount, baseCenter);
}
