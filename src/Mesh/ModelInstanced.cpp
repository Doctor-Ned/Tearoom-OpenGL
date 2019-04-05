#include "ModelInstanced.h"
#include "Model.h"

ModelInstanced::ModelInstanced(std::vector<ModelData*> data, glm::vec3* offsets, int offsetSize) : Mesh(STNone) {
	this->offsets = offsets;
	this->offsetSize = offsetSize;
	for(auto &modelData : data) {
		meshes.push_back(new MeshModelInstanced(modelData->vertices, modelData->indices, modelData->textures, offsets, offsetSize));
	}
}

void ModelInstanced::draw(glm::mat4 world) {
	for(auto &mesh : meshes) {
		static_cast<Mesh*>(mesh)->draw(world);
	}
}

void ModelInstanced::draw(Shader *shader, glm::mat4 world) {
	for (auto& mesh : meshes) {
		mesh->draw(shader, world);
	}
}