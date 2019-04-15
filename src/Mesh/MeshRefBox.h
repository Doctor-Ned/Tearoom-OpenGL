#ifndef MESHREFBOX_H
#define MESHREFBOX_H

#include "Global.h"
#include "MeshRef.h"

class MeshRefBox : public MeshRef {
public:
	MeshRefBox(bool reflective, glm::vec3 dimensions);
	MeshRefBox(bool reflective, glm::vec3 min, glm::vec3 max);
	void updateValues(glm::vec3 min, glm::vec3 max);
	glm::vec3 getMin() const;
	glm::vec3 getMax() const;
protected:
	void draw(Shader *shader, glm::mat4 world) override;
	glm::vec3 getUnmodeledCenter() override;
	void setupMesh();
	glm::vec3 min, max;
	unsigned int vertexAmount;
};

#endif
