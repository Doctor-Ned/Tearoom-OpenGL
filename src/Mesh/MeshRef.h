#ifndef MESHREF_H
#define MESHREF_H

#include "Mesh.h"

class MeshRef abstract: public Mesh {
public:
	void regenEnvironmentMap(glm::mat4 model, std::function<void(glm::mat4, glm::mat4)> renderCallback);
	GLuint getEnvironmentMap() const;
	SerializableType getSerializableType() override;
protected:
	void draw(Shader *shader, glm::mat4 model) override;
	virtual glm::vec3 getUnmodeledCenter() = 0;
	MeshRef(bool reflective);
	GLuint environmentMap;
	GLuint fbo, rb;
	friend class Scene;
};

#endif
