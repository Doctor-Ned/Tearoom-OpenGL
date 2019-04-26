#include "Model.h"
#include <stb_image.h>

Model::Model(std::string path) : Model(AssetManager::getInstance()->getModelData(path)) {
	this->path = path;
}

Model::Model(std::vector<ModelData*> data) : Mesh(STModel) {
	initialize(data);
}

void Model::draw(Shader *shader, glm::mat4 world) {
	for (auto& mesh : meshes) {
		mesh->drawSelf(shader, world);
	}
}

std::vector<ModelData*> Model::createModelData(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("Assimp could not properly load the model. %s\n", importer.GetErrorString());
		exit(1);
	}
	std::string directory = path.substr(0, path.find_last_of('/'));
	std::vector<ModelData*> result;
	processNode(scene->mRootNode, scene, directory, result);
	return result;
}

void Model::setOpacity(float opacity) {
	Mesh::setOpacity(opacity);
	for(auto &mesh : meshes) {
		mesh->setOpacity(opacity);
	}
}

void Model::setCulled(bool culled) {
	Mesh::setCulled(culled);
	for(auto &mesh : meshes) {
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

void Model::initialize(std::vector<ModelData*> data) {
	for (auto &modelData : data) {
		meshes.push_back(new MeshModel(modelData->vertices, modelData->indices, modelData->textures));
	}
}

void Model::processNode(aiNode* node, const aiScene* scene, const std::string& directory, std::vector<ModelData*> &output) {
	std::vector<ModelTexture> textures_loaded;
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		output.push_back(processMesh(mesh, scene, directory, textures_loaded));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, directory, output);
	}
}

ModelData *Model::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory, std::vector<ModelTexture> &textures_loaded) {
	ModelData *result = new ModelData();
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
		}
		else
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
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<ModelTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory, textures_loaded);
	result->textures.insert(result->textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	std::vector<ModelTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory, textures_loaded);
	result->textures.insert(result->textures.end(), specularMaps.begin(), specularMaps.end());

	std::vector<ModelTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", directory, textures_loaded);
	result->textures.insert(result->textures.end(), normalMaps.begin(), normalMaps.end());

	std::vector<ModelTexture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", directory, textures_loaded);
	result->textures.insert(result->textures.end(), heightMaps.begin(), heightMaps.end());

	return result;
}

std::vector<ModelTexture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const std::string& directory, std::vector<ModelTexture> &textures_loaded) {
	std::vector<ModelTexture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			ModelTexture texture;
			texture.id = textureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

GLuint Model::textureFromFile(const char* path, const std::string& directory) {
	std::string filename = std::string(path);
	filename = directory + '\\' + filename;

	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		printf("Failed to load texture from file '%s'!\n", path);
		stbi_image_free(data);
	}

	return textureID;
}
