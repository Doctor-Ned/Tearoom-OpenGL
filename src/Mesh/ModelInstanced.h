#ifndef MODELINSTANCED_H
#define MODELINSTANCED_H

#include "MeshModelInstanced.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>
#include <string>

class ModelInstanced : public Mesh {
public:
	ModelInstanced(std::vector<ModelData*> data, glm::vec3 *offsets, int offsetSize);
	void setOpacity(float opacity) override;
	void setCulled(bool culled) override;
private:
	void draw(Shader *shader, glm::mat4 world) override;
	glm::vec3* offsets;
	int offsetSize;
	std::vector<MeshModelInstanced*> meshes;
};

#endif
