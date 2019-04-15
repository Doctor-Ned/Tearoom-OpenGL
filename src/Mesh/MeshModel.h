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
};

struct ModelTexture {
	GLuint id;
	std::string type;
	std::string path;
};

class MeshModel : public Mesh {
public:
	MeshModel(std::vector<ModelVertex> vertices, std::vector<unsigned int> indices,
	          std::vector<ModelTexture> textures);
protected:
	void draw(Shader *shader, glm::mat4 world) override;
	void setupMesh();
	std::vector<unsigned int> indices;
	std::vector<ModelVertex> vertices;
	std::vector<ModelTexture> textures;
};

#endif
