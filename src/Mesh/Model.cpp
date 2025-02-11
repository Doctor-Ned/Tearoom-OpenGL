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

void Model::setEmissiveFactor(float emissiveFactor) {
	Mesh::setEmissiveFactor(emissiveFactor);
	for(auto &mesh : meshes) {
		mesh->setEmissiveFactor(emissiveFactor);
	}
}

void Model::setDepthScale(float depth)
{
	Mesh::setDepthScale(depth);
	for (auto &mesh : meshes) {
		mesh->setDepthScale(depth);
	}
}

void Model::setTileX(float tile)
{
	Mesh::setTileX(tile);
	for (auto &mesh : meshes) {
		mesh->setTileX(tile);
	}
}

void Model::setTileY(float tile)
{
	Mesh::setTileY(tile);
	for (auto &mesh : meshes) {
		mesh->setTileY(tile);
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
	this->path = root.get("model", "").asString();
	initialize(AssetManager::getInstance()->getModelData(path));
	Mesh::deserialize(root, serializer);
}

void Model::renderGui() {
	Mesh::renderGui();
	ImGui::Text(path.c_str());
}

void Model::initialize(ModelData* data) {
	for (auto &modelNodeData : data->nodeData) {
		meshes.push_back(new MeshModel(modelNodeData->vertices, modelNodeData->indices, data->textures));
	}
	setEmissiveFactor(data->textures[2].id == 0 ? 0.0f : 1.0f);
}