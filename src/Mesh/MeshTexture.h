#ifndef MESHTEXTURE_H
#define MESHTEXTURE_H

#include "Mesh.h"
#include "Scene/AssetManager.h"

struct TextureVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class MeshTexture : public Mesh {
public:
protected:
	void draw(Shader *shader, glm::mat4 world) override;
	MeshTexture(std::vector<TextureVertex> vertices, std::vector<unsigned int> indices,
	            char* textureFile);
	MeshTexture();
	Texture texture;
	std::vector<unsigned int> indices;
	std::vector<TextureVertex> vertices;
};

#endif
