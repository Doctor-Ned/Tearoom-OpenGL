#ifndef MESHREF_H
#define MESHREF_H

#include "Mesh.h"

class MeshRef : public Mesh {
public:
	void regenEnvironmentMap(glm::mat4 model, std::function<void(glm::mat4, glm::mat4)> renderCallback, GLuint framebuffer);
	void draw(Shader *shader, glm::mat4 model) override;
	GLuint getEnvironmentMap() const;
protected:
	virtual glm::vec3 getUnmodeledCenter() = 0;
	MeshRef(bool reflective);
	GLuint environmentMap;
	GLuint fbo, rb;
};

#endif
