#include "ModelInstanced.h"
#include "Model.h"

ModelInstanced::ModelInstanced(std::vector<ModelData*> data, glm::vec3* offsets, int offsetSize) : Mesh(STModelInstanced) {
	this->offsets = offsets;
	this->offsetSize = offsetSize;
	for(auto &modelData : data) {
		meshes.push_back(new MeshModelInstanced(modelData->vertices, modelData->indices, modelData->textures, offsets, offsetSize));
	}
}

void ModelInstanced::draw(Shader *shader, glm::mat4 world) {
	for (auto& mesh : meshes) {
		mesh->drawSelf(shader, world);
	}
}

void ModelInstanced::setOpacity(float opacity) {
	Mesh::setOpacity(opacity);
	for(auto &mesh: meshes) {
		mesh->setOpacity(opacity);
	}
}

void ModelInstanced::setCulled(bool culled) {
	Mesh::setCulled(culled);
	for (auto &mesh : meshes) {
		mesh->setCulled(culled);
	}
}