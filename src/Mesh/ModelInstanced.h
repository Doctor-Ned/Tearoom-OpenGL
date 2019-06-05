#ifndef MODELINSTANCED_H
#define MODELINSTANCED_H

#include "MeshModelInstanced.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>
#include <string>

class ModelInstanced : public Mesh {
public:
	ModelInstanced(std::string path, glm::vec3 *offsets, int offsetSize);
	void setOpacity(float opacity) override;
	void setCulled(bool culled) override;
	void setUseLight(bool useLight) override;
	void setCastShadows(bool castShadows) override;
	void setRenderMode(GLuint renderMode) override;
	void setOpaque(bool opaque) override;
	void setEmissiveFactor(float emissiveFactor) override;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
private:
	ModelInstanced() {}
	void renderGui() override;
	void initialize(ModelData* data, glm::vec3 *offsets, int offsetSize);
	std::string path;
	ModelInstanced(ModelData* data, glm::vec3 *offsets, int offsetSize);
	void draw(Shader *shader, glm::mat4 world) override;
	glm::vec3* offsets;
	int offsetSize;
	std::vector<MeshModelInstanced*> meshes;
	friend class Serializer;
};

#endif
