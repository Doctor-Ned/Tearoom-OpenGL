#ifndef MESHSIMPLE_H
#define MESHSIMPLE_H

#include "Headers.h"
#include "Mesh.h"
#include <string>
#include <vector>

struct SimpleVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
};

class MeshSimple : public Mesh {
public:
	MeshSimple(Shader shader, std::vector<SimpleVertex> vertices, std::vector<unsigned int> indices);
	void draw(Shader shader, glm::mat4 world, float scale = 1.0f) override;
	void setColor(glm::vec4 color);
protected:
	MeshSimple(Shader shader, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	void setupMesh() override;
	glm::vec4 color;
	std::vector<SimpleVertex> vertices;
};

#endif
