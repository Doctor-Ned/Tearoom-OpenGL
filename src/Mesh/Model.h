#ifndef MODEL_H
#define MODEL_H

#include "MeshModel.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>

class Model : public Mesh {
public:
	Model(Shader shader, char* path);
	void draw(glm::mat4 world, float scale = 1.0f);
	void draw(Shader shader, glm::mat4 world, float scale = 1.0f) override;
private:
	void setupMesh() override;
	std::vector<ModelTexture> textures_loaded;
	std::vector<MeshModel> meshes;
	std::string directory;
	void loadModel(std::string const& path);
	void processNode(aiNode* node, const aiScene* scene);
	MeshModel processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<ModelTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	GLuint TextureFromFile(const char* path, const std::string& directory);
};

#endif
