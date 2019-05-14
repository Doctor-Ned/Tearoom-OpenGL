#ifndef MESHMODEL_H
#define MESHMODEL_H

#include "Global.h"
#include "Mesh.h"
#include <string>
#include <vector>

struct ModelVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	int bonesIDs[4] = {0};
	float bonesWages[4] = {0.0f};
	int boneCounter = 0;
};

struct AnimModelVertex : ModelVertex
{
	
};

struct ModelTexture {
	ModelTexture() : id(0), path("") {}
	GLuint id;
	std::string path;
};

class MeshModel : public Mesh {
public:
	MeshModel(std::vector<ModelVertex> vertices, std::vector<unsigned int> indices, Texture textures[6]);
protected:
	void draw(Shader *shader, glm::mat4 world) override;
	void setupMesh();
	std::vector<unsigned int> indices;
	std::vector<ModelVertex> vertices;
	Texture textures[6];
};

#endif
