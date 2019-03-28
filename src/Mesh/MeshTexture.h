#ifndef MESHTEXTURE_H
#define MESHTEXTURE_H

#include "Mesh.h"

struct TextureVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class MeshTexture : public Mesh {
public:
	MeshTexture(Shader shader, std::vector<TextureVertex> vertices, std::vector<unsigned int> indices,
	            char* textureFile);
	void draw(Shader shader, glm::mat4 world, float scale = 1.0f) override;
protected:
	MeshTexture(Shader shader);
	void setupMesh() override;
	Texture texture;
	std::vector<TextureVertex> vertices;
};

#endif
