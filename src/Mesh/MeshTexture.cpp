#include "MeshTexture.h"

SerializableType MeshTexture::getSerializableType() {
	return SMeshTexture;
}

Json::Value MeshTexture::serialize(Serializer* serializer) {
	Json::Value root = Mesh::serialize(serializer);
	root["texture"] = texture.path;
	return root;
}

void MeshTexture::deserialize(Json::Value& root, Serializer* serializer) {
	Mesh::deserialize(root, serializer);
	texture = AssetManager::getInstance()->getTexture(root.get("texture", "").asString());
}

void MeshTexture::draw(Shader *shader, glm::mat4 world) {
	Mesh::draw(shader, world);
}

MeshTexture::MeshTexture() : Mesh(STTexture) { }
