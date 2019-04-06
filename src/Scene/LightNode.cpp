#include "LightNode.h"

void LightNode::updateWorld() {
	GraphNode::updateWorld();
	//if (dirty) {
		setModel(worldTransform.Matrix());
	//}
}

void LightNode::renderGui() {
	GraphNode::renderGui();
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
