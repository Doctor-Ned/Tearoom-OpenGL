#include "MeshBox.h"
#include "Serialization/DataSerializer.h"

MeshBox::MeshBox(glm::vec3 dimensions, std::string texturePath) :
	MeshBox(glm::vec3(-dimensions.x / 2.0f, -dimensions.y / 2.0f, -dimensions.z / 2.0f), glm::vec3(dimensions.x / 2.0f, dimensions.y / 2.0f, dimensions.z / 2.0f), texturePath) {}

MeshBox::MeshBox(glm::vec3 min, glm::vec3 max, std::string texturePath)
	: MeshTexture(), min(min), max(max) {
	texture = AssetManager::getInstance()->getTexture(texturePath);
	setupMesh();
}

void MeshBox::renderGui() {
	MeshTexture::renderGui();
	static bool useDimensions = false;
	ImGui::Checkbox("Use dimensions", &useDimensions);
	if (useDimensions) {
		glm::vec3 dimensions = getDimensions();
		glm::vec3 oldDim = dimensions;
		ImGui::DragFloat3("Dimensions", reinterpret_cast<float*>(&dimensions), 0.1f, 0.0f, std::numeric_limits<float>::max());
		ImGui::InputFloat3("Dimensions (fixed)", reinterpret_cast<float*>(&dimensions));
		for (int i = 0; i < 3; i++) {
			if (dimensions[i] < 0.0f) {
				dimensions[i] = 0.0f;
			}
		}
		if (dimensions != oldDim) {
			updateValues(dimensions);
		}
	} else {
		glm::vec3 min = this->min;
		glm::vec3 max = this->max;
		ImGui::DragFloat3("Min", reinterpret_cast<float*>(&min), 0.1f, 0.0f, std::numeric_limits<float>::max());
		ImGui::InputFloat3("Min (fixed)", reinterpret_cast<float*>(&min));
		ImGui::DragFloat3("Max", reinterpret_cast<float*>(&max), 0.1f, 0.0f, std::numeric_limits<float>::max());
		ImGui::InputFloat3("Max (fixed)", reinterpret_cast<float*>(&max));
		for (int i = 0; i < 3; i++) {
			if (min != this->min) {
				if (min[i] > max[i]) {
					min[i] = max[i];
				}
			} else if (max != this->max) {
				if (max[i] < min[i]) {
					max[i] = min[i];
				}
			}
		}
		if (min != this->min || max != this->max) {
			updateValues(min, max);
		}
	}
}

void MeshBox::draw(Shader *shader, glm::mat4 world) {
	MeshTexture::draw(shader, world);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glBindVertexArray(VAO);
	glBindVertexBuffer(0, VBO, 0, sizeof(TextureVertex));
	glDrawArrays(renderMode, 0, vertexAmount);
	glBindVertexArray(0);

}

void MeshBox::updateValues(glm::vec3 min, glm::vec3 max) {
	this->min = min;
	this->max = max;

	TextureVertex vertices[24];

	glm::vec3 normal(0.0f, 1.0f, 0.0f); //top

	vertices[0].Normal = normal; //bl
	vertices[1].Normal = normal; //ul
	vertices[2].Normal = normal; //ur
	vertices[3].Normal = normal; //br

	vertices[0].TexCoords = glm::vec2(0.0f, 1.0f);
	vertices[1].TexCoords = glm::vec2(0.0f, 0.0f);
	vertices[2].TexCoords = glm::vec2(1.0f, 0.0f);
	vertices[3].TexCoords = glm::vec2(1.0f, 1.0f);

	vertices[0].Position = glm::vec3(min.x, max.y, max.z);
	vertices[1].Position = glm::vec3(min.x, max.y, min.z);
	vertices[2].Position = glm::vec3(max.x, max.y, min.z);
	vertices[3].Position = glm::vec3(max.x, max.y, max.z);

	normal = glm::vec3(0.0f, -1.0f, 0.0f); //bottom

	vertices[4].Normal = normal;
	vertices[5].Normal = normal;
	vertices[6].Normal = normal;
	vertices[7].Normal = normal;

	vertices[4].TexCoords = vertices[0].TexCoords;
	vertices[5].TexCoords = vertices[1].TexCoords;
	vertices[6].TexCoords = vertices[2].TexCoords;
	vertices[7].TexCoords = vertices[3].TexCoords;

	vertices[4].Position = vertices[1].Position;
	vertices[4].Position.y = min.y;
	vertices[5].Position = vertices[0].Position;
	vertices[5].Position.y = min.y;
	vertices[6].Position = vertices[3].Position;
	vertices[6].Position.y = min.y;
	vertices[7].Position = vertices[2].Position;
	vertices[7].Position.y = min.y;

	normal = glm::vec3(0.0f, 0.0f, 1.0f); //front

	vertices[8].Normal = normal;
	vertices[9].Normal = normal;
	vertices[10].Normal = normal;
	vertices[11].Normal = normal;

	vertices[8].TexCoords = vertices[0].TexCoords;
	vertices[9].TexCoords = vertices[1].TexCoords;
	vertices[10].TexCoords = vertices[2].TexCoords;
	vertices[11].TexCoords = vertices[3].TexCoords;

	vertices[8].Position = vertices[0].Position;
	vertices[8].Position.y = min.y;
	vertices[9].Position = vertices[0].Position;
	vertices[10].Position = vertices[3].Position;
	vertices[11].Position = vertices[3].Position;
	vertices[11].Position.y = min.y;

	normal = glm::vec3(0.0f, 0.0f, -1.0f); //back

	vertices[12].Normal = normal;
	vertices[13].Normal = normal;
	vertices[14].Normal = normal;
	vertices[15].Normal = normal;

	vertices[12].TexCoords = vertices[0].TexCoords;
	vertices[13].TexCoords = vertices[1].TexCoords;
	vertices[14].TexCoords = vertices[2].TexCoords;
	vertices[15].TexCoords = vertices[3].TexCoords;

	vertices[12].Position = vertices[9].Position;
	vertices[12].Position.z = min.z;
	vertices[13].Position = vertices[8].Position;
	vertices[13].Position.z = min.z;
	vertices[14].Position = vertices[11].Position;
	vertices[14].Position.z = min.z;
	vertices[15].Position = vertices[10].Position;
	vertices[15].Position.z = min.z;

	normal = glm::vec3(-1.0f, 0.0f, 0.0f); //left

	vertices[16].Normal = normal;
	vertices[17].Normal = normal;
	vertices[18].Normal = normal;
	vertices[19].Normal = normal;

	vertices[16].TexCoords = vertices[0].TexCoords;
	vertices[17].TexCoords = vertices[1].TexCoords;
	vertices[18].TexCoords = vertices[2].TexCoords;
	vertices[19].TexCoords = vertices[3].TexCoords;

	vertices[16].Position = vertices[1].Position;
	vertices[16].Position.y = min.y;
	vertices[17].Position = vertices[1].Position;
	vertices[18].Position = vertices[0].Position;
	vertices[19].Position = vertices[0].Position;
	vertices[19].Position.y = min.y;

	normal = glm::vec3(1.0f, 0.0f, 0.0f); //right

	vertices[20].Normal = normal;
	vertices[21].Normal = normal;
	vertices[22].Normal = normal;
	vertices[23].Normal = normal;

	vertices[20].TexCoords = vertices[0].TexCoords;
	vertices[21].TexCoords = vertices[1].TexCoords;
	vertices[22].TexCoords = vertices[2].TexCoords;
	vertices[23].TexCoords = vertices[3].TexCoords;

	vertices[20].Position = vertices[17].Position;
	vertices[20].Position.x = max.x;
	vertices[21].Position = vertices[16].Position;
	vertices[21].Position.x = max.x;
	vertices[22].Position = vertices[19].Position;
	vertices[22].Position.x = max.x;
	vertices[23].Position = vertices[18].Position;
	vertices[23].Position.x = max.x;

	std::vector<TextureVertex> data;
	for (int i = 0; i < 24; i += 4) {
		data.push_back(vertices[i]);
		data.push_back(vertices[i + 2]);
		data.push_back(vertices[i + 1]);
		data.push_back(vertices[i]);
		data.push_back(vertices[i + 3]);
		data.push_back(vertices[i + 2]);
	}

	vertexAmount = 36;

	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
	}

	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexAmount * sizeof(TextureVertex), &data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), static_cast<void*>(nullptr));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), reinterpret_cast<void*>(offsetof(TextureVertex, Normal)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), reinterpret_cast<void*>(offsetof(TextureVertex, TexCoords)));

	glBindVertexArray(0);
	data.clear();
}

void MeshBox::updateValues(glm::vec3 dimensions) {
	updateValues(glm::vec3(-dimensions.x / 2.0f, -dimensions.y / 2.0f, -dimensions.z / 2.0f), glm::vec3(dimensions.x / 2.0f, dimensions.y / 2.0f, dimensions.z / 2.0f));
}

glm::vec3 MeshBox::getMin() const {
	return min;
}

glm::vec3 MeshBox::getMax() const {
	return max;
}

glm::vec3 MeshBox::getDimensions() const {
	glm::vec3 result;
	result.x = abs(max.x - min.x);
	result.y = abs(max.y - min.y);
	result.z = abs(max.z - min.z);
	return result;
}

SerializableType MeshBox::getSerializableType() {
	return SMeshBox;
}

Json::Value MeshBox::serialize(Serializer* serializer) {
	Json::Value root = MeshTexture::serialize(serializer);
	root["min"] = DataSerializer::serializeVec3(min);
	root["max"] = DataSerializer::serializeVec3(max);
	return root;
}

void MeshBox::deserialize(Json::Value& root, Serializer* serializer) {
	MeshTexture::deserialize(root, serializer);
	min = DataSerializer::deserializeVec3(root["min"]);
	max = DataSerializer::deserializeVec3(root["max"]);
	setupMesh();
}

void MeshBox::setupMesh() {
	glGenVertexArrays(1, &VAO);
	updateValues(min, max);
}
