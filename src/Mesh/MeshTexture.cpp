#include "MeshTexture.h"

MeshTexture::MeshTexture(std::vector<TextureVertex> vertices, std::vector<unsigned int> indices,
                         char* textureFile)
	: Mesh(), indices(indices), vertices(vertices) {
	this->shader = SceneManager::getInstance()->getShader(getShaderType());
	texture = createTexture(textureFile);
}

void MeshTexture::draw(Shader *shader, glm::mat4 world, float scale) {
	Mesh::draw(shader, world, scale);
}

ShaderType MeshTexture::getShaderType() {
	return STTexture;
}

MeshTexture::MeshTexture() : Mesh() {
	this->shader = SceneManager::getInstance()->getShader(getShaderType());
}
