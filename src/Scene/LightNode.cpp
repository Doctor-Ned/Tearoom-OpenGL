#include "LightNode.h"

void LightNode::renderGui() {
	GraphNode::renderGui();
	if (active) {
		bool enabled = getEnabled();
		glm::vec4 ambient = getAmbient(), diffuse = getDiffuse(), specular = getSpecular();
		ImGui::Checkbox("Light enabled", &enabled);
		ImGui::NewLine();
		setEnabled(enabled);
		if (enabled) {
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
}