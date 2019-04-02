#include "PointLightNode.h"

PointLightNode::
PointLightNode(PointLight* light, Mesh* mesh, GraphNode* parent) : GraphNode(mesh, parent), light(light) {
	constant = light->constant;
	linear = light->linear;
	quadratic = light->quadratic;
	pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lastEnabled = true;
}

void PointLightNode::update(double timeDiff) {
	//light->model = getWorld();
	light->model = worldTransform.Matrix();
	GraphNode::update(timeDiff);
}

void PointLightNode::drawGui(bool autoUpdate) {
	ImGui::PushID((uintptr_t)this);
	ImGui::Checkbox("PointLight enabled", &lastEnabled);
	ImGui::NewLine();
	if (lastEnabled) {
		ImGui::SliderFloat3("PointLight position", (float*)&pos, -10.0f, 10.0f);
		ImGui::NewLine();
		ImGui::SliderFloat("PointLight constant", &constant, 0.0f, 10.0f);
		ImGui::NewLine();
		ImGui::SliderFloat("PointLight linear", &linear, 0.0f, 10.0f);
		ImGui::NewLine();
		ImGui::SliderFloat("PointLight quadratic", &quadratic, 0.0f, 10.0f);
		ImGui::NewLine();
		ImGui::SliderFloat("Near plane", &light->near_plane, 0.01f, 100.0f);
		ImGui::NewLine();
		ImGui::SliderFloat("Far plane", &light->far_plane, 0.01f, 100.0f);
		ImGui::NewLine();
		ImGui::ColorEdit3("PointLight color", (float*)&(light->diffuse));
		ImGui::NewLine();
	}

	if (autoUpdate || ImGui::Button("Apply PointLight changes")) {
		light->constant = constant;
		light->linear = linear;
		light->quadratic = quadratic;
		//local = translate(glm::mat4(1.0f), glm::vec3(light->position));
		localTransform.SetMatrix(glm::mat4(1));
		localTransform.Translate(glm::vec3(pos));
		dirty = true;
		//light->model = getWorld();
		light->model = worldTransform.Matrix();

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

PointLight* PointLightNode::getLight() {
	return light;
}
