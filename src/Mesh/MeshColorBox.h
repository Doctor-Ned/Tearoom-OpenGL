#ifndef MESHCOLORBOX_H
#define MESHCOLORBOX_H

#include "Global.h"
#include "MeshSimple.h"

class MeshColorBox : public MeshSimple {
public:
	MeshColorBox(glm::vec3 min, glm::vec3 max, glm::vec4 color);
	void draw(Shader *shader, glm::mat4 world) override;
	void updateValues(glm::vec3 min, glm::vec3 max);
	glm::vec3 getMin();
	glm::vec3 getMax();
protected:
	void setupMesh();
	glm::vec3 min, max;
	unsigned int vertexAmount;
};

#endif
