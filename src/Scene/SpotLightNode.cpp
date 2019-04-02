#include "SpotLightNode.h"

SpotLightNode::SpotLightNode(SpotLight* light, Mesh* mesh, GraphNode* parent) : GraphNode(mesh, parent), light(light) {
	rotationZ = 0.0f;
	rotationX = 0.0f;
	appliedZ = 0.0f;
	appliedX = 0.0f;
	constant = light->constant;
	linear = light->linear;
	quadratic = light->quadratic;
	cutOff = light->cutOff;
	outerCutOff = light->outerCutOff;
	lastEnabled = true;
}

void SpotLightNode::update(double timeDiff) {
	//light->model = getWorld();
	light->model = worldTransform.Matrix();
	GraphNode::update(timeDiff);
}

void SpotLightNode::drawGui(bool autoUpdate) {
	ImGui::PushID((uintptr_t)this);
	ImGui::Checkbox("SpotLight enabled", &lastEnabled);
	ImGui::NewLine();
	if (lastEnabled) {
		ImGui::SliderAngle("SpotLight rotationZ", &rotationZ);
		ImGui::NewLine();
		ImGui::SliderAngle("SpotLight rotationX", &rotationX);
		ImGui::NewLine();
		ImGui::SliderFloat3("SpotLight position", (float*)&pos, -10.0f, 10.0f);
		ImGui::NewLine();
		ImGui::SliderFloat("SpotLight constant", &constant, 0.0f, 10.0f);
		ImGui::NewLine();
		ImGui::SliderFloat("SpotLight linear", &linear, 0.0f, 10.0f);
		ImGui::NewLine();
		ImGui::SliderFloat("SpotLight quadratic", &quadratic, 0.0f, 10.0f);
		ImGui::NewLine();
		if (outerCutOff < cutOff) {
			outerCutOff = cutOff;
		}
		ImGui::SliderAngle("SpotLight cutOff", &cutOff, 0.0f, 180.0f);
		ImGui::NewLine();
		ImGui::SliderAngle("SpotLight outerCutOff", &outerCutOff, cutOff, 180.0f);
		ImGui::NewLine();
		ImGui::ColorEdit3("SpotLight color", (float*)&(light->diffuse));
		ImGui::NewLine();
	}

	if (autoUpdate || ImGui::Button("Apply SpotLight changes")) {
		light->constant = constant;
		light->linear = linear;
		light->quadratic = quadratic;
		light->cutOff = cutOff;
		light->outerCutOff = outerCutOff;
		/*local = translate(glm::mat4(1.0f), glm::vec3(light->position));
		local = rotate(local, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
		local = rotate(local, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));*/
		localTransform.SetMatrix(glm::mat4(1));
		localTransform.Translate(glm::vec3(pos));
		localTransform.RotateByRadians(rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
		localTransform.RotateByRadians(rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
		appliedZ = rotationZ;
		appliedX = rotationX;
		dirty = true;
		//light->model = getWorld();
		light->model = worldTransform.Matrix();
		//}
		if (lastEnabled != enabled) {
			enabled = lastEnabled;
			if (enabled) {
				light->ambient = lastAmbient;
				light->diffuse = lastDiffuse;
				light->specular = lastSpecular;
			}
			else {
				lastAmbient = light->ambient;
				lastDiffuse = light->diffuse;
				lastSpecular = light->specular;
				light->ambient = glm::vec4(0.0f);
				light->diffuse = glm::vec4(0.0f);
				light->specular = glm::vec4(0.0f);
			}
		}
	}
	ImGui::PopID();
}

SpotLight* SpotLightNode::getLight() {
	return light;
}
