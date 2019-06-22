#include "AnimatedModel.h"
#include <stb_image.h>
#include <fstream>
#include <iostream>
#include "Scene/Components/Animation.h"
#include "glm/gtx/quaternion.hpp"
#include "Bone.h"
#include "Global.h"
#include "Scene/Components/KeyFrameAnimation.h"
#include "Scene/Components/SkeletalAnimation.h"


AnimatedModel::AnimatedModel(std::string path) : AnimatedModel(AssetManager::getInstance()->getAnimatedModelData(path), AssetManager::getInstance()->getBoneHierarchy(path)) {
	this->path = path;
}

AnimatedModel::AnimatedModel(AnimatedModelData* data, Bone *boneHierarchy) : Mesh(STAnimatedModel) {
	initialize(data, boneHierarchy);
}

Bone* AnimatedModel::getBoneRoot() {
	return rootBone;
}

std::vector<SkeletalAnimation> AnimatedModel::loadAnimations(aiAnimation** anims, int animCount) {
	std::vector<SkeletalAnimation> animations;
	for (int i = 0; i < animCount; i++) {
		animations.emplace_back(SkeletalAnimation(nullptr, anims[i]->mName.C_Str()));
		double ticksPerSecond = anims[i]->mTicksPerSecond;
		double animationDuration = anims[i]->mDuration / ticksPerSecond;

		for (unsigned int j = 0; j < anims[i]->mNumChannels; j++) {
			const char* nodeName = anims[i]->mChannels[j]->mNodeName.C_Str();
			float keyTime;
			//position
			for (unsigned int k = 0; k < anims[i]->mChannels[j]->mNumPositionKeys; k++) {
				keyTime = anims[i]->mChannels[j]->mPositionKeys[k].mTime / ticksPerSecond;
				aiVector3D assimpPos = anims[i]->mChannels[j]->mPositionKeys[k].mValue;
				glm::vec3 pos(assimpPos.x, assimpPos.y, assimpPos.z);
				animations[i].addKeyFrame(nodeName, anim::TRANSLATION, keyTime, pos);
			}
			//scale
			for (unsigned int k = 0; k < anims[i]->mChannels[j]->mNumScalingKeys; k++) {
				keyTime = anims[i]->mChannels[j]->mScalingKeys[k].mTime / ticksPerSecond;
				aiVector3D assimpScale = anims[i]->mChannels[j]->mScalingKeys[k].mValue;
				glm::vec3 scale(assimpScale.x, assimpScale.y, assimpScale.z);
				animations[i].addKeyFrame(nodeName, anim::SCALE, keyTime, scale);
			}
			//rotation
			for (unsigned int k = 0; k < anims[i]->mChannels[j]->mNumRotationKeys; k++) {
				keyTime = anims[i]->mChannels[j]->mRotationKeys[k].mTime / ticksPerSecond;
				aiQuaternion q = anims[i]->mChannels[j]->mRotationKeys[k].mValue;
				glm::quat q2 = glm::quat(q.w, q.x, q.y, q.z);
				//glm::vec3 eulerAngles = Global::radiansToDegrees(glm::eulerAngles(q2));
				animations[i].addKeyFrame(nodeName, anim::ROTATION, keyTime, q2);
			}
		}
	}
	return animations;
}

Bone* AnimatedModel::loadBones(const aiScene* scene, AnimatedModelData& data) {

	int boneID = 0;
	int meshesCount = scene->mNumMeshes;
	//bones vector through all the meshes
	std::vector<Bone*> bones;
	for (int i = 0; i < meshesCount; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		int bonesCount = mesh->mNumBones;
		for (int j = 0; j < bonesCount; j++) {
			
			aiBone* b = mesh->mBones[j];
			Bone* bone;
			auto it = std::find_if(bones.begin(), bones.end(), [b](const Bone* bone) { return b->mName.C_Str() == bone->name; });
			if (it != bones.end())
			{
				bone = *it;
			}
			else
			{
				bone = new Bone();
				bone->ID = boneID;
				bone->name = b->mName.C_Str();
				bone->inverseBindPose = Global::aiMatrix4x4ToGlm(b->mOffsetMatrix);
				boneID++;
				bones.push_back(bone);
			}

			{
				int verticesCount = b->mNumWeights;
				for (int k = 0; k < verticesCount; k++) {
					int vertexIndex = b->mWeights[k].mVertexId;
					int boneIndex = data.nodeData[i]->vertices[vertexIndex].BoneCounter;
					data.nodeData[i]->vertices[vertexIndex].BoneIDs[boneIndex] = bone->ID;
					data.nodeData[i]->vertices[vertexIndex].BoneWages[boneIndex] = b->mWeights[k].mWeight;
					data.nodeData[i]->vertices[vertexIndex].BoneCounter++;
				}
			}
		}
	}

	aiNode* root = scene->mRootNode;
	Bone* boneRoot = nullptr;
	for (int i = 0; i < root->mNumChildren; i++) {
		auto it = std::find_if(bones.begin(), bones.end(), [i, root](const Bone* b) { return root->mChildren[i]->mName.C_Str() == b->name; });
		if (it != bones.end()) {
			boneRoot = *it;
			boneRoot->transform = Global::aiMatrix4x4ToGlm(root->mChildren[i]->mTransformation);
			recreateBonesHierarchy(boneRoot, root->mChildren[i], bones);
		}
	}
	if (boneRoot) {
		//boneRoot->setID();
		boneRoot->toString();
	}
	return boneRoot;
}

void AnimatedModel::recreateBonesHierarchy(Bone* parent, aiNode* currentSceneNode, std::vector<Bone*>& bones) {
	for (int i = 0; i < currentSceneNode->mNumChildren; i++) {
		aiNode* child = currentSceneNode->mChildren[i];
		auto it = std::find_if(bones.begin(), bones.end(), [&child](const Bone* b) { return child->mName.C_Str() == b->name; });
		if (it != bones.end()) {
			Bone* bone = *it;
			bone->parent = parent;
			parent->children.push_back(bone);
			bone->transform = Global::aiMatrix4x4ToGlm(currentSceneNode->mChildren[i]->mTransformation);
			recreateBonesHierarchy(bone, currentSceneNode->mChildren[i], bones);
		}
	}
}

void AnimatedModel::addToBoneTransformMatrix(Bone* bone, glm::mat4(&boneTransforms)[MAX_BONE_TRANSFORMS]) {
	boneTransforms[bone->ID] = bone->worldTransform.getMatrix() * bone->inverseBindPose;
	for (auto &b : bone->children) {
		addToBoneTransformMatrix(b, boneTransforms);
	}
}

void AnimatedModel::draw(Shader *shader, glm::mat4 world) {
	if (rootBone) {
		rootBone->updateWorld();
		glm::mat4 boneTransforms[MAX_BONE_TRANSFORMS];
		addToBoneTransformMatrix(rootBone, boneTransforms);
		shader->setAnimatedModelBoneTransforms(boneTransforms);
	}
	for (auto& mesh : meshes) {
		mesh->drawSelf(shader, world);
	}
}

FullModelData* AnimatedModel::createModelData(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		SPDLOG_ERROR("Assimp could not properly load the model '{}': {}!", path, importer.GetErrorString());
		exit(1);
	}
	std::string directory = path.substr(0, path.find_last_of('/'));
	FullModelData* result = new FullModelData();
	Texture *textures = loadModelTextures(path);
	for (int i = 0; i < 7; i++) {
		result->animatedData.textures[i] = textures[i];
		result->simpleData.textures[i] = textures[i];
	}
	delete[] textures;
	
	auto anims = loadAnimations(scene->mAnimations, scene->mNumAnimations);
	processNode(scene->mRootNode, scene, directory, &result->animatedData);
	Bone* boneRoot = loadBones(scene, result->animatedData);

	for (auto &dat : result->animatedData.nodeData) {
		ModelNodeData *data = new ModelNodeData();
		data->indices = dat->indices;
		for (auto &ver : dat->vertices) {
			ModelVertex vertex;
			vertex.Position = ver.Position;
			vertex.Normal = ver.Normal;
			vertex.TexCoords = ver.TexCoords;
			vertex.Tangent = ver.Tangent;
			vertex.Bitangent = ver.Bitangent;
			data->vertices.push_back(vertex);
		}
		result->simpleData.nodeData.push_back(data);
	}

	glm::mat4 globalInverseMatrix = Global::aiMatrix4x4ToGlm(scene->mRootNode->mTransformation);

	AssetManager::getInstance()->addBoneHierarchy(path, boneRoot);
	for (auto& animation : anims) {
		AssetManager::getInstance()->addAnimation(path, animation);
	}
	/*anims[0].setRootBone(boneRoot);
	anims[0].play();
	anims[0].update(0.016f);*/

	result->animatedData.checkBoneCounters();
	return result;
}

void AnimatedModel::setOpacity(float opacity) {
	Mesh::setOpacity(opacity);
	for (auto &mesh : meshes) {
		mesh->setOpacity(opacity);
	}
}

void AnimatedModel::setCulled(bool culled) {
	Mesh::setCulled(culled);
	for (auto &mesh : meshes) {
		mesh->setCulled(culled);
	}
}

void AnimatedModel::setUseLight(bool useLight) {
	Mesh::setUseLight(useLight);
	for (auto &mesh : meshes) {
		mesh->setUseLight(useLight);
	}
}

void AnimatedModel::setCastShadows(bool castShadows) {
	Mesh::setCastShadows(castShadows);
	for (auto &mesh : meshes) {
		mesh->setCastShadows(castShadows);
	}
}

void AnimatedModel::setRenderMode(GLuint renderMode) {
	Mesh::setRenderMode(renderMode);
	for (auto &mesh : meshes) {
		mesh->setRenderMode(renderMode);
	}
}

void AnimatedModel::setOpaque(bool opaque) {
	Mesh::setOpaque(opaque);
	for (auto &mesh : meshes) {
		mesh->setOpaque(opaque);
	}
}

SerializableType AnimatedModel::getSerializableType() {
	return SAnimatedModel;
}

Json::Value AnimatedModel::serialize(Serializer* serializer) {
	Json::Value root = Mesh::serialize(serializer);
	root["model"] = path;
	return root;
}

void AnimatedModel::deserialize(Json::Value& root, Serializer* serializer) {
	Mesh::deserialize(root, serializer);
	this->path = root.get("model", "").asString();
	initialize(AssetManager::getInstance()->getAnimatedModelData(path), AssetManager::getInstance()->getBoneHierarchy(path));
}

void AnimatedModel::renderGui() {
	Mesh::renderGui();
	ImGui::Text(path.c_str());
}

void AnimatedModel::initialize(AnimatedModelData* data, Bone *boneHierarchy) {
	for (auto &modelNodeData : data->nodeData) {
		meshes.push_back(new MeshAnimatedModel(modelNodeData->vertices, modelNodeData->indices, data->textures));
	}
	if (boneHierarchy != nullptr) {
		rootBone = new Bone(boneHierarchy);
		rootBone->setParentsForChildren();
	} else {
		rootBone = nullptr;
	}
}

void AnimatedModel::processNode(aiNode* node, const aiScene* scene, const std::string& directory, AnimatedModelData* output) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		output->nodeData.push_back(processMesh(mesh, scene, directory));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, directory, output);
	}
}

AnimatedModelNodeData *AnimatedModel::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory) {
	AnimatedModelNodeData *result = new AnimatedModelNodeData();
	// Walk through each of the mesh's vertices
	union assimpToGlmVec {
		glm::vec3* vec3;
		aiVector3D* vec;
	};

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		AnimatedModelVertex vertex;
		//glm::vec3 vector;
		assimpToGlmVec u;
		u.vec = mesh->mVertices + i;
		vertex.Position = *u.vec3;

		u.vec = mesh->mNormals + i;
		vertex.Normal = *u.vec3;

		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		} else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		u.vec = mesh->mTangents + i;
		vertex.Tangent = *u.vec3;

		u.vec = mesh->mBitangents + i;
		vertex.Bitangent = *u.vec3;

		result->vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			result->indices.push_back(face.mIndices[j]);
	}
	return result;
}

Texture *AnimatedModel::loadModelTextures(const std::string &objPath) {
	Texture *textures = new Texture[7];
	AssetManager *assetManager = AssetManager::getInstance();
	std::string dir = objPath.substr(0, objPath.find_last_of("/") + 1);
	for (auto &text : assetManager->getTextures()) {
		if (Global::startsWith(text, dir)) {
			if (Global::endsWith(text, "_AO.png")) {
				textures[0] = assetManager->getTexture(text);
			} else if (Global::endsWith(text, "_BaseColor.png")) {
				textures[1] = assetManager->getTexture(text);
			} else if (Global::endsWith(text, "_Emissive.png")) {
				textures[2] = assetManager->getTexture(text);
			} else if (Global::endsWith(text, "_Metallic.png")) {
				textures[3] = assetManager->getTexture(text);
			} else if (Global::endsWith(text, "_Normal.png")) {
				textures[4] = assetManager->getTexture(text);
			} else if (Global::endsWith(text, "_Roughness.png")) {
				textures[5] = assetManager->getTexture(text);
			} else if (Global::endsWith(text, "_Height.png")) {
				textures[6] = assetManager->getTexture(text);
			}
		}
	}
	return textures;
}