#ifndef MESHSIMPLE_H
#define MESHSIMPLE_H

#include "Global.h"
#include "Mesh.h"
#include <string>
#include <vector>

struct SimpleVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
};

class MeshSimple : public Mesh {
public:
	void draw(Shader *shader, glm::mat4 world, float scale = 1.0f) override;
	ShaderType getShaderType() override;
	void setColor(glm::vec4 color);
protected:
	MeshSimple(glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	glm::vec4 color;
	std::vector<SimpleVertex> vertices;
};

#endif
