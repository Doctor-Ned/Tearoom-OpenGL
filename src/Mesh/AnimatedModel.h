#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/anim.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include "Model.h"
#include "MeshAnimatedModel.h"

struct AnimatedModelNodeData {
	std::vector<AnimatedModelVertex> vertices;
	std::vector<unsigned int> indices;
};

struct AnimatedModelData {
	std::vector<AnimatedModelNodeData*> nodeData;
	Texture textures[7];

	void checkBoneCounters()
	{
		for(auto& nData : nodeData)
		{
			for(auto& vertex : nData->vertices)
			{
				if(vertex.BoneCounter > 4)
				{
					float d = 4;
				}
				if (vertex.BoneCounter > 5)
				{
					float d = 4;
				}
				if (vertex.BoneCounter > 6)
				{
					float d = 4;
				}
			}
		}
	}
};

struct FullModelData {
	ModelData simpleData;
	AnimatedModelData animatedData;
};

class SkeletalAnimation;
class Bone;
class AnimatedModel : public Mesh {
public:
	AnimatedModel(std::string path);
	static FullModelData* createModelData(std::string path);
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
	AnimatedModel(AnimatedModelData* data, Bone *boneHierarchy);
	void renderGui() override;
	void initialize(AnimatedModelData* data, Bone *boneHierarchy);
	std::string path;
	Bone* rootBone;
	static std::vector<SkeletalAnimation> loadAnimations(aiAnimation** anims, int animCount);
	static Bone* loadBones(const aiScene* scene, AnimatedModelData& data);
	static void recreateBonesHierarchy(Bone* parent, aiNode* currentSceneNode, std::vector<Bone*>& bones);
	void addToBoneTransformMatrix(Bone* bone, glm::mat4 (&boneTransforms)[MAX_BONE_TRANSFORMS]);
	void draw(Shader *shader, glm::mat4 world) override;
	std::vector<MeshAnimatedModel*> meshes;
	static void processNode(aiNode* node, const aiScene* scene, const std::string& directory, AnimatedModelData* output);
	static AnimatedModelNodeData *processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
	static Texture* loadModelTextures(const std::string& objPath);
	friend class Serializer;
};