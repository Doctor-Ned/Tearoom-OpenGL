#include "Model.h"
#include <stb_image.h>
#include <fstream>

Model::Model(std::string path) : Model(AssetManager::getInstance()->getModelData(path)) {
	this->path = path;
}

Model::Model(ModelData* data) : Mesh(STModel) {
	initialize(data);
}

void Model::draw(Shader *shader, glm::mat4 world) {
	for (auto& mesh : meshes) {
		mesh->drawSelf(shader, world);
	}
}

ModelData* Model::createModelData(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("Assimp could not properly load the model. %s\n", importer.GetErrorString());
		exit(1);
	}
	std::string directory = path.substr(0, path.find_last_of('/'));
	ModelData* result = new ModelData();
	Texture *textures = loadModelTextures(path);
	for(int i=0;i<6;i++ ) {
		result->textures[i] = textures[i];
	}
	delete[] textures;
	processNode(scene->mRootNode, scene, directory, result);
	return result;
}

void Model::setOpacity(float opacity) {
	Mesh::setOpacity(opacity);
	for (auto &mesh : meshes) {
		mesh->setOpacity(opacity);
	}
}

void Model::setCulled(bool culled) {
	Mesh::setCulled(culled);
	for (auto &mesh : meshes) {
		mesh->setCulled(culled);
	}
}

void Model::setUseLight(bool useLight) {
	Mesh::setUseLight(useLight);
	for (auto &mesh : meshes) {
		mesh->setUseLight(useLight);
	}
}

void Model::setCastShadows(bool castShadows) {
	Mesh::setCastShadows(castShadows);
	for (auto &mesh : meshes) {
		mesh->setCastShadows(castShadows);
	}
}

void Model::setRenderMode(GLuint renderMode) {
	Mesh::setRenderMode(renderMode);
	for (auto &mesh : meshes) {
		mesh->setRenderMode(renderMode);
	}
}

void Model::setOpaque(bool opaque) {
	Mesh::setOpaque(opaque);
	for (auto &mesh : meshes) {
		mesh->setOpaque(opaque);
	}
}

SerializableType Model::getSerializableType() {
	return SModel;
}

Json::Value Model::serialize(Serializer* serializer) {
	Json::Value root = Mesh::serialize(serializer);
	root["model"] = path;
	return root;
}

void Model::deserialize(Json::Value& root, Serializer* serializer) {
	Mesh::deserialize(root, serializer);
	this->path = root.get("model", "").asString();
	initialize(AssetManager::getInstance()->getModelData(path));
}

void Model::renderGui() {
	Mesh::renderGui();
	ImGui::Text(path.c_str());
}

void Model::initialize(ModelData* data) {
	for (auto &modelNodeData : data->nodeData) {
		meshes.push_back(new MeshModel(modelNodeData->vertices, modelNodeData->indices, data->textures));
	}
}

void Model::processNode(aiNode* node, const aiScene* scene, const std::string& directory, ModelData* output) {
	std::vector<ModelTexture> textures_loaded;
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		output->nodeData.push_back(processMesh(mesh, scene, directory));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, directory, output);
	}
}

ModelNodeData *Model::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory) {
	ModelNodeData *result = new ModelNodeData();
	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		ModelVertex vertex;
		glm::vec3 vector;

		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		} else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.Tangent = vector;

		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		vertex.Bitangent = vector;
		result->vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			result->indices.push_back(face.mIndices[j]);
	}
	return result;
}

Texture *Model::loadModelTextures(const std::string &objPath) {
	Texture *textures = new Texture[6];
	AssetManager *assetManager = AssetManager::getInstance();
	textures[0] = textureFromFile(Global::getReplace(objPath, ".obj", "_default_AO.png"));
	textures[1] = textureFromFile(Global::getReplace(objPath, ".obj", "_default_BaseColor.png"));
	textures[2] = textureFromFile(Global::getReplace(objPath, ".obj", "_default_Emissive.png"));
	textures[3] = textureFromFile(Global::getReplace(objPath, ".obj", "_default_Metallic.png"));
	textures[4] = textureFromFile(Global::getReplace(objPath, ".obj", "_default_Normal.png"));
	textures[5] = textureFromFile(Global::getReplace(objPath, ".obj", "_default_Roughness.png"));
	return textures;
}

Texture Model::textureFromFile(const std::string &path) {
	std::ifstream infile(path);
	if(infile.good()) {
		return AssetManager::getInstance()->getTexture(path);
	}
	return Texture();
}
