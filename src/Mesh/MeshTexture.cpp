#include "MeshTexture.h"

MeshTexture::MeshTexture(std::vector<TextureVertex> vertices, std::vector<unsigned int> indices,
                         char* textureFile)
	: Mesh(STTexture), indices(indices), vertices(vertices) {
	texture = AssetManager::getInstance()->getTexture(textureFile);
}

void MeshTexture::draw(Shader *shader, glm::mat4 world) {
	Mesh::draw(shader, world);
}

MeshTexture::MeshTexture() : Mesh(STTexture) { }
