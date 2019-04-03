#include "MeshTexture.h"

MeshTexture::MeshTexture(std::vector<TextureVertex> vertices, std::vector<unsigned int> indices,
                         char* textureFile)
	: Mesh(), indices(indices), vertices(vertices) {
	this->shader = AssetManager::getInstance()->getShader(getShaderType());
	texture = AssetManager::getInstance()->getTexture(textureFile);
}

void MeshTexture::draw(Shader *shader, glm::mat4 world) {
	Mesh::draw(shader, world);
}

ShaderType MeshTexture::getShaderType() {
	return STTexture;
}

MeshTexture::MeshTexture() : Mesh() {
	this->shader = AssetManager::getInstance()->getShader(getShaderType());
}
