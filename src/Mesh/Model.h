#ifndef MODEL_H
#define MODEL_H

#include "MeshModel.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>

struct ModelData {
	std::vector<ModelVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<ModelTexture> textures;
};

class Model : public Mesh {
public:
	Model(std::string path);
	Model(std::vector<ModelData*> data);
	void draw(glm::mat4 world) override;
	void draw(Shader *shader, glm::mat4 world) override;
	static std::vector<ModelData*> createModelData(std::string path);
	void setOpacity(float opacity) override;
private:
	std::vector<MeshModel*> meshes;
	static void processNode(aiNode* node, const aiScene* scene, const std::string& directory, std::vector<ModelData*> &output);
	static ModelData *processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory, std::vector<ModelTexture> &textures_loaded);
	static std::vector<ModelTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const std::string& directory, std::vector<ModelTexture> &textures_loaded);
	static GLuint textureFromFile(const char* path, const std::string& directory);
};

#endif
