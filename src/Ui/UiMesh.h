#pragma once

#include "UiElement.h"

class Mesh;

class UiMesh : public UiElement {
public:
	UiMesh(glm::vec2 position, Mesh *mesh = nullptr);
	void render(Shader *shader) override;
	void setup() override;
	//when using setMesh, remember to use updateElement() on the scene with this UiMesh as a parameter!
	void setMesh(Mesh *mesh);
	Mesh *getMesh();
	ShaderType getShaderType() override;
protected:
	void updateWorld() override;
	Mesh *mesh = nullptr;
};