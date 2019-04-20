#include "LightComp.h"
#include "Scene/GraphNode.h"

LightComp::LightComp(GraphNode* gameObject) : Component(gameObject) {}
void LightComp::updateWorld() {
	setModel(gameObject->worldTransform.getMatrix());
}

void LightComp::setComponentActive(bool active) {
	Component::setComponentActive(active);
	setEnabled(active);
}

void LightComp::renderGui() {
	Component::renderGui();
	if (active) {
		glm::vec4 ambient = getAmbient(), diffuse = getDiffuse(), specular = getSpecular();
		ImGui::ColorEdit4("Ambient", &ambient[0]);
		ImGui::NewLine();
		ImGui::ColorEdit4("Diffuse", &diffuse[0]);
		ImGui::NewLine();
		ImGui::ColorEdit4("Specular", &specular[0]);
		ImGui::NewLine();
		setAmbient(ambient);
		setDiffuse(diffuse);
		setSpecular(specular);
	}
}