#include "MeshTexture.h"

MeshTexture::MeshTexture(Shader *shader, std::vector<TextureVertex> vertices, std::vector<unsigned int> indices,
                         char* textureFile)
	: Mesh(shader), indices(indices), vertices(vertices) {
	texture = createTexture(textureFile);
}

void MeshTexture::draw(Shader *shader, glm::mat4 world, float scale) {
	Mesh::draw(shader, world, scale);
}

MeshTexture::MeshTexture(Shader *shader) : Mesh(shader) {}