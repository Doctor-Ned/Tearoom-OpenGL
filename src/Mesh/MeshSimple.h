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

class MeshSimple abstract: public Mesh {
public:
	void setColor(glm::vec4 color);
	bool isOpaque() const override;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	void renderGui() override;
	void draw(Shader *shader, glm::mat4 world) override;
	MeshSimple(glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	glm::vec4 color;
	std::vector<SimpleVertex> vertices;
};

#endif
