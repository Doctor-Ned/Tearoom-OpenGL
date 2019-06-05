#include "ModelInstanced.h"
#include "Model.h"
#include "Serialization/DataSerializer.h"

ModelInstanced::ModelInstanced(ModelData* data, glm::vec3* offsets, int offsetSize) : Mesh(STModelInstanced) {
	initialize(data, offsets, offsetSize);
}

void ModelInstanced::draw(Shader *shader, glm::mat4 world) {
	for (auto& mesh : meshes) {
		mesh->drawSelf(shader, world);
	}
}

ModelInstanced::ModelInstanced(std::string path, glm::vec3* offsets, int offsetSize) : ModelInstanced(AssetManager::getInstance()->getModelData(path), offsets, offsetSize) {
	this->path = path;
}

void ModelInstanced::setOpacity(float opacity) {
	Mesh::setOpacity(opacity);
	for (auto &mesh : meshes) {
		mesh->setOpacity(opacity);
	}
}

void ModelInstanced::setCulled(bool culled) {
	Mesh::setCulled(culled);
	for (auto &mesh : meshes) {
		mesh->setCulled(culled);
	}
}

void ModelInstanced::setUseLight(bool useLight) {
	Mesh::setUseLight(useLight);
	for (auto &mesh : meshes) {
		mesh->setUseLight(useLight);
	}
}

void ModelInstanced::setCastShadows(bool castShadows) {
	Mesh::setCastShadows(castShadows);
	for (auto &mesh : meshes) {
		mesh->setCastShadows(castShadows);
	}
}

void ModelInstanced::setRenderMode(GLuint renderMode) {
	Mesh::setRenderMode(renderMode);
	for (auto &mesh : meshes) {
		mesh->setRenderMode(renderMode);
	}
}

void ModelInstanced::setOpaque(bool opaque) {
	Mesh::setOpaque(opaque);
	for (auto &mesh : meshes) {
		mesh->setOpaque(opaque);
	}
}

void ModelInstanced::setEmissiveFactor(float emissiveFactor) {
	Mesh::setEmissiveFactor(emissiveFactor);
	for(auto &mesh : meshes) {
		mesh->setEmissiveFactor(emissiveFactor);
	}
}

SerializableType ModelInstanced::getSerializableType() {
	return SModelInstanced;
}

Json::Value ModelInstanced::serialize(Serializer* serializer) {
	Json::Value root = Mesh::serialize(serializer);
	root["model"] = path;
	for(int i=0;i<offsetSize;i++) {
		root["offsets"][i] = DataSerializer::serializeVec3(offsets[i]);
	}
	return root;
}

void ModelInstanced::deserialize(Json::Value& root, Serializer* serializer) {
	int offsetSize = root["offsets"].size();
	glm::vec3 *offsets = new glm::vec3[offsetSize];
	for(int i=0;i<offsetSize;i++) {
		offsets[i] = DataSerializer::deserializeVec3(root["offsets"][i]);
	}
	this->path = root.get("model", "").asString();
	initialize(AssetManager::getInstance()->getModelData(path), offsets, offsetSize);
	Mesh::deserialize(root, serializer);
}

void ModelInstanced::renderGui() {
	Mesh::renderGui();
	ImGui::Text(path.c_str());
}

void ModelInstanced::initialize(ModelData* data, glm::vec3* offsets, int offsetSize) {
	this->offsets = offsets;
	this->offsetSize = offsetSize;
	for (auto &modelNodeData : data->nodeData) {
		meshes.push_back(new MeshModelInstanced(modelNodeData->vertices, modelNodeData->indices, data->textures, offsets, offsetSize));
	}
	setEmissiveFactor(data->textures[2].id == 0 ? 0.0f : 1.0f);
}
