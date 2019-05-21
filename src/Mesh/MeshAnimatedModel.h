#pragma once

#include "Global.h"
#include "Mesh.h"
#include <string>
#include <vector>

struct AnimatedModelVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	int BoneIDs[8] = {0};
	float BoneWages[8] = {0.0f};
	int BoneCounter = 0;
};

class MeshAnimatedModel : public Mesh {
public:
	MeshAnimatedModel(std::vector<AnimatedModelVertex> vertices, std::vector<unsigned int> indices, Texture textures[6]);
protected:
	void draw(Shader *shader, glm::mat4 world) override;
	void setupMesh();
	std::vector<unsigned int> indices;
	std::vector<AnimatedModelVertex> vertices;
	Texture textures[6];
};