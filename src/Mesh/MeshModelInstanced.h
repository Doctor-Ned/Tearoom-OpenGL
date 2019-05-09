#ifndef MESHMODELINSTANCED_H
#define MESHMODELINSTANCED_H

#include "Global.h"
#include "MeshModel.h"
#include <string>
#include <vector>

class MeshModelInstanced : public Mesh {
public:
	MeshModelInstanced(std::vector<ModelVertex> vertices, std::vector<unsigned int> indices, Texture textures[6], glm::vec3* offsets, int offsetSize);
protected:
	void draw(Shader *shader, glm::mat4 world) override;
	void setupMesh();
	glm::vec3* offsets;
	int offsetSize;
	GLuint instanceVBO;
	std::vector<unsigned int> indices;
	std::vector<ModelVertex> vertices;
	Texture textures[6];
};

#endif
