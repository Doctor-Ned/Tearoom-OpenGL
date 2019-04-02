#ifndef MODELINSTANCED_H
#define MODELINSTANCED_H

#include "MeshModelInstanced.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>
#include <string>

class ModelInstanced : public Mesh {
public:
	ModelInstanced(char* path, glm::vec3* offsets, int offsetSize);
	ShaderType getShaderType() override;
	void draw(Shader *shader, glm::mat4 world) override;
private:
	glm::vec3* offsets;
	int offsetSize;
	std::vector<ModelTexture> textures_loaded;
	std::vector<MeshModelInstanced*> meshes;
	std::string directory;
	void loadModel(std::string const& path);
	void processNode(aiNode* node, const aiScene* scene);
	MeshModelInstanced *processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<ModelTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	GLuint TextureFromFile(const char* path, const std::string& directory);
};

#endif
