#ifndef MESHREFBOX_H
#define MESHREFBOX_H

#include "Global.h"
#include "MeshRef.h"

class MeshRefBox : public MeshRef {
public:
	MeshRefBox(bool reflective, glm::vec3 dimensions);
	MeshRefBox(bool reflective, glm::vec3 min, glm::vec3 max);
	void updateValues(glm::vec3 min, glm::vec3 max);
	void updateValues(glm::vec3 dimensions);
	glm::vec3 getMin() const;
	glm::vec3 getMax() const;
	glm::vec3 getDimensions() const;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	MeshRefBox() : MeshRef(false) {}
	void draw(Shader *shader, glm::mat4 world) override;
	glm::vec3 getUnmodeledCenter() override;
	void setupMesh();
	glm::vec3 min, max;
	unsigned int vertexAmount;
	friend class Serializer;
};

#endif
