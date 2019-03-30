#ifndef MESHMODEL_H
#define MESHMODEL_H

#include "Headers.h"
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
	void draw(Shader *shader, glm::mat4 world, float scale = 1.0f) override;
	ShaderType getShaderType() override;
protected:
	void setupMesh();
	std::vector<unsigned int> indices;
	std::vector<ModelVertex> vertices;
	std::vector<ModelTexture> textures;
};

#endif
