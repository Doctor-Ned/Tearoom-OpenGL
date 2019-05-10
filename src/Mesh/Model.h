#ifndef MODEL_H
#define MODEL_H

#include "MeshModel.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>


struct ModelNodeData {
	std::vector<ModelVertex> vertices;
	std::vector<unsigned int> indices;
};

struct ModelData {
	std::vector<ModelNodeData*> nodeData;
	Texture textures[6];
};

class Model : public Mesh {
public:
	Model(std::string path);
	static ModelData* createModelData(std::string path);
	void setOpacity(float opacity) override;
	void setCulled(bool culled) override;
	void setUseLight(bool useLight) override;
	void setCastShadows(bool castShadows) override;
	void setRenderMode(GLuint renderMode) override;
	void setOpaque(bool opaque) override;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
private:
	Model() {}
	void renderGui() override;
	void initialize(ModelData* data);
	std::string path;
	Model(ModelData* data);
	void draw(Shader *shader, glm::mat4 world) override;
	std::vector<MeshModel*> meshes;
	static void processNode(aiNode* node, const aiScene* scene, const std::string& directory, ModelData* output);
	static ModelNodeData *processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
	static Texture* loadModelTextures(const std::string& objPath);
	friend class Serializer;
};

#endif
