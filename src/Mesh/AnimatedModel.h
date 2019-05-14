#ifndef ANIMATED_MODEL_H
#define ANIMATED_MODEL_H

#include "MeshModel.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/anim.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include "Model.h"

struct VertexForAnimation
{
	
};
class SkeletalAnimation;
class Bone;
class AnimatedModel : public Mesh {
public:
	AnimatedModel(std::string path);
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
	Bone* getBoneRoot();
private:
	AnimatedModel() {}
	AnimatedModel(ModelData* data);
	void renderGui() override;
	void initialize(ModelData* data);
	std::string path;
	Bone* rootBone;
	static std::vector<SkeletalAnimation> loadAnimations(aiAnimation** anims, int animCount);
	static Bone* loadBones(const aiScene* scene);
	static void recreateBonesHierarchy(Bone* parent, aiNode* currentSceneNode, std::vector<Bone*>& bones);
	static void assignBonesToVertices(const Bone* root, ModelData* data);
	void draw(Shader *shader, glm::mat4 world) override;
	std::vector<MeshModel*> meshes;
	static void processNode(aiNode* node, const aiScene* scene, const std::string& directory, ModelData* output);
	static ModelNodeData *processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
	static Texture* loadModelTextures(const std::string& objPath);
	friend class Serializer;
};

#endif
