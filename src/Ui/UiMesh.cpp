#include "UiMesh.h"
#include "Scene/AssetManager.h"
#include "Mesh/Mesh.h"

UiMesh::UiMesh(glm::vec2 position, Mesh* mesh) : UiElement(position, glm::vec2(0.0f, 0.0f)) {
	setMesh(mesh);
	localTransform.setRotationDegrees(glm::vec3(45.0f, 45.0f, 0.0f));
}

void UiMesh::render(Shader* shader) {
	bool castShadows = mesh->getCastShadows(), useLight = mesh->getUseLight();
	mesh->setCastShadows(false);
	mesh->setUseLight(false);
	mesh->drawSelf(shader, worldTransform.getMatrix());
	mesh->setCastShadows(castShadows);
	mesh->setUseLight(useLight);
}

void UiMesh::setup() {}

void UiMesh::setMesh(Mesh* mesh) {
	this->mesh = mesh;
	if (mesh != nullptr) {
		localTransform.setScale(mesh->getUiScale());
	}
}

Mesh* UiMesh::getMesh() {
	return mesh;
}

ShaderType UiMesh::getShaderType() {
	return mesh == nullptr ? STNone : mesh->getShaderType();
}

void UiMesh::updateWorld() {
	if (mesh != nullptr) {
		glm::vec3 scale = localTransform.getData().scale;
		float scl = scale[0];
		if (scl != scale[1] || scl != scale[2] || scl != mesh->getUiScale()) {
			localTransform.setScale(mesh->getUiScale());
		}
	}
	UiElement::updateWorld();
}
