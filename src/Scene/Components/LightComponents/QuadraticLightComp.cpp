#include "QuadraticLightComp.h"
SerializableType QuadraticLightComp::getSerializableType() {
	return SQuadraticLightComp;
}

void QuadraticLightComp::renderGui() {
	LightComp::renderGui();
	if(active) {
		float constant = getConstant(), linear = getLinear(), quadratic = getQuadratic();
		ImGui::SliderFloat("Constant", &constant, 0.001f, 100.0f);
		ImGui::NewLine();
		ImGui::SliderFloat("Linear", &linear, 0.001f, 100.0f);
		ImGui::NewLine();
		ImGui::SliderFloat("Quadratic", &quadratic, 0.001f, 100.0f);
		ImGui::NewLine();
		setConstant(constant);
		setLinear(linear);
		setQuadratic(quadratic);
	}
}
