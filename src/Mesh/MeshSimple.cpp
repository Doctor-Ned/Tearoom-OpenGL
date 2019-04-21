#include "MeshSimple.h"
#include "Serialization/DataSerializer.h"

MeshSimple::MeshSimple(glm::vec4 color) : Mesh(STColor), color(color) { }

void MeshSimple::draw(Shader *shader, glm::mat4 world) {
	Mesh::draw(shader, world);
	shader->setColor(color);
}

void MeshSimple::setColor(glm::vec4 color) {
	this->color = color;
}

bool MeshSimple::isOpaque() const {
	return color.a > 0.99f && Mesh::isOpaque();
}

SerializableType MeshSimple::getSerializableType() {
	return SMeshSimple;
}

Json::Value MeshSimple::serialize(Serializer* serializer) {
	Json::Value root = Mesh::serialize(serializer);
	root["color"] = DataSerializer::serializeVec4(color);
	return root;
}

void MeshSimple::deserialize(Json::Value& root, Serializer* serializer) {
	Mesh::deserialize(root, serializer);
	color = DataSerializer::deserializeVec4(root.get("color", DataSerializer::serializeVec4(color)));
}
