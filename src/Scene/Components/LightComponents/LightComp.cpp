#include "LightComp.h"
#include "Scene/GraphNode.h"
#include "Serialization/DataSerializer.h"
#include "Mesh/MeshSimple.h"

LightComp::LightComp(GraphNode* gameObject) : Component(gameObject) {}
void LightComp::updateWorld() {
	setModel(gameObject->worldTransform.getMatrix());
}

void LightComp::setComponentActive(bool active) {
	Component::setComponentActive(active);
	setEnabled(active);
	if (overrideMeshColor) {
		if (active) {
			setMeshColor(getColor());
		} else {
			setMeshColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		}
	}
}

void LightComp::setColor(glm::vec4 color) {
	if (overrideMeshColor && getEnabled()) {
		setMeshColor(color);
	}
}

SerializableType LightComp::getSerializableType() {
	return SLightComp;
}

bool LightComp::getOverrideMeshColor() {
	return overrideMeshColor;
}

void LightComp::setOverrideMeshColor(bool overrideColor) {
	this->overrideMeshColor = overrideColor;
	if(overrideColor) {
		setMeshColor(getEnabled() ? getColor() : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
}

void LightComp::setMeshColor(glm::vec4 color) {
	MeshSimple *mesh = dynamic_cast<MeshSimple*>(gameObject->getMesh());
	if (mesh) {
		mesh->setColor(color);
		mesh->setEmissive(color);
	}
}

void LightComp::renderGui() {
	Component::renderGui();
	if (active) {
		glm::vec4 color = getColor();
		ImGui::ColorEdit4("Color", &color[0]);
		bool overrideColor = getOverrideMeshColor();
		float n = getNearPlane(), f = getFarPlane();
		ImGui::DragFloat("Near plane", &n, 0.01f);
		ImGui::DragFloat("Far plane", &f, 0.01f);
		if (n < 0.0f) {
			n = 0.0f;
		} else if(n > f) {
			n = f;
		}
		if (f < n) {
			f = n;
		}
		setNearPlane(n);
		setFarPlane(f);
		ImGui::Checkbox("Override mesh color", &overrideColor);
		if(overrideColor != getOverrideMeshColor()) {
			setOverrideMeshColor(overrideColor);
		}
		setColor(color);
	}
}
