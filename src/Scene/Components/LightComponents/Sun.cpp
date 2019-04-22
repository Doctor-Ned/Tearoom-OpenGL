#include "Sun.h"
#include "Render/LightManager.h"
#include "Mesh/MeshColorSphere.h"
#include "Scene/GraphNode.h"
#include "Serialization/Serializer.h"

Sun::Sun(DirLight* light1, DirLight* light2, glm::vec4 dawnColor, glm::vec4 dayColor, glm::vec4 duskColor,
	glm::vec4 nightColor, float sunDistance, float initialTime, GraphNode *gameObject, DirLightComp *dirLightComp1, DirLightComp *dirLightComp2) : Component(gameObject, "Sun"), light1(light1), light2(light2),
	dawnColor(dawnColor), dayColor(dayColor), duskColor(duskColor), nightColor(nightColor), sunDistance(sunDistance), light1Comp(dirLightComp1), light2Comp(dirLightComp2) {
	setTime(initialTime);
}

Sun::Sun(DirLight* light1, DirLight* light2, glm::vec4 dawnColor, glm::vec4 dayColor, glm::vec4 duskColor,
	glm::vec4 nightColor, float sunDistance, float initialTime, GraphNode *gameObject) : Component(gameObject, "Sun"), light1(light1), light2(light2),
	dawnColor(dawnColor), dayColor(dayColor), duskColor(duskColor), nightColor(nightColor), sunDistance(sunDistance) {
	setTime(initialTime);
	GraphNode *light1Node = new GraphNode(new MeshColorSphere(0.25f, 30, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)), gameObject);
	light1Node->getMesh()->setShaderType(STLight);
	GraphNode *light2Node = new GraphNode(new MeshColorSphere(0.25f, 30, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)), gameObject);
	light2Node->getMesh()->setShaderType(STLight);
	light1Comp = new DirLightComp(light1, light1Node);
	light1Node->addComponent(light1Comp);
	light2Comp = new DirLightComp(light2, light2Node);
	light2Node->addComponent(light2Comp);
	light1Comp->getGameObject()->localTransform.setMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -sunDistance)));
	light1Comp->getGameObject()->localTransform.rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	light2Comp->getGameObject()->localTransform.setMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, sunDistance)));
}

void Sun::setTime(float time) {
	this->time = time;
	dirty = true;
}

void Sun::addTime(float time) {
	this->time = this->time + time;
	dirty = true;
}

void Sun::setVerticalRotation(float angle) {
	this->rotationAngle = angle;
	dirty = true;
}

float Sun::getTime() {
	return time;
}

void Sun::update(float msec) {
	if (dirty) {
		glm::vec4 light1Color = timeToColor(time, true), light2Color = timeToColor(time, false);
		light1->ambient = light1Color * ambientFactor;
		light1->diffuse = light1Color;
		light1->specular = light1Color * specularFactor;
		light2->ambient = light2Color * ambientFactor;
		light2->diffuse = light2Color;
		light2->specular = light2Color * specularFactor;
		dynamic_cast<MeshSimple*>(light1Comp->getGameObject()->getMesh())->setColor(light1Color);
		dynamic_cast<MeshSimple*>(light2Comp->getGameObject()->getMesh())->setColor(light2Color);
		glm::vec4 position = gameObject->localTransform.getMatrix()[3];
		gameObject->localTransform.setMatrix(rotate(translate(glm::mat4(1.0f), glm::vec3(position)), glm::radians(rescaleTime(time - 6.0f) * 360.0f / 24.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	}
}

void Sun::renderGui() {
	Component::renderGui();
	if(active) {
		ImGui::SliderFloat("Time", &time, -24.0f, 24.0f);
		ImGui::NewLine();
		ImGui::SliderFloat("Ambient factor", &ambientFactor, 0.0f ,1.0f);
		ImGui::NewLine();
		ImGui::SliderFloat("Specular factor", &specularFactor, 0.0f, 1.0f);
		ImGui::NewLine();
		ImGui::ColorEdit3("Dawn color", reinterpret_cast<float*>(&dawnColor));
		ImGui::NewLine();
		ImGui::ColorEdit3("Day color", reinterpret_cast<float*>(&dayColor));
		ImGui::NewLine();
		ImGui::ColorEdit3("Dusk color", reinterpret_cast<float*>(&duskColor));
		ImGui::NewLine();
		ImGui::ColorEdit3("Night color", reinterpret_cast<float*>(&nightColor));
		ImGui::NewLine();
		dirty = true;
	}
}

void Sun::setComponentActive(bool active) {
	Component::setComponentActive(active);
	light1Comp->setComponentActive(active);
	light2Comp->setComponentActive(active);
}

float Sun::getAmbientFactor() {
	return ambientFactor;
}

void Sun::setAmbientFactor(float ambientFactor) {
	this->ambientFactor = ambientFactor;
	dirty = true;
}

float Sun::getSpecularFactor() {
	return specularFactor;
}

void Sun::setSpecularFactor(float specularFactor) {
	this->specularFactor = specularFactor;
	dirty = true;
}

SerializableType Sun::getSerializableType() {
	return SSun;
}

Json::Value Sun::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["ambientFactor"] = ambientFactor;
	root["specularFactor"] = specularFactor;
	root["time"] = time;
	root["rotationAngle"] = rotationAngle;
	root["dawnColor"] = DataSerializer::serializeVec4(dawnColor);
	root["dayColor"] = DataSerializer::serializeVec4(dayColor);
	root["duskColor"] = DataSerializer::serializeVec4(duskColor);
	root["nightColor"] = DataSerializer::serializeVec4(nightColor);
	root["light1"] = serializer->serialize(light1);
	root["light2"] = serializer->serialize(light2);
	root["light1Comp"] = serializer->serialize(light1Comp);
	root["light2Comp"] = serializer->serialize(light2Comp);
	root["sunDistance"] = sunDistance;
	return root;
}

void Sun::deserialize(Json::Value& root, Serializer* serializer) {
	light1 = dynamic_cast<DirLight*>(serializer->deserialize(root["light1"]).object);
	light2 = dynamic_cast<DirLight*>(serializer->deserialize(root["light2"]).object);
	light1Comp = dynamic_cast<DirLightComp*>(serializer->deserialize(root["light1Comp"]).object);
	light2Comp = dynamic_cast<DirLightComp*>(serializer->deserialize(root["light2Comp"]).object);
	Component::deserialize(root, serializer);
	ambientFactor = root["ambientFactor"].asFloat();
	specularFactor = root["specularFactor"].asFloat();
	time = root["time"].asFloat();
	rotationAngle = root["rotationAngle"].asFloat();
	dawnColor = DataSerializer::deserializeVec4(root["dawnColor"]);
	dayColor = DataSerializer::deserializeVec4(root["dayColor"]);
	duskColor = DataSerializer::deserializeVec4(root["duskColor"]);
	nightColor = DataSerializer::deserializeVec4(root["nightColor"]);
	sunDistance = root["sunDistance"].asFloat();
	dirty = true;
}

float Sun::rescaleTime(float time) {
	while (time >= 24.0f) {
		time -= 24.0f;
	}
	while (time < 0.0f) {
		time += 24.0f;
	}
	return time;
}

glm::vec4 Sun::timeToColor(float time, bool light1) {
	time = rescaleTime(time);
	int quarter = static_cast<int>(time) / 6;
	time -= quarter * 6;
	float factor1 = time / 6.0f;
	float factor2 = 1.0f - factor1;
	if (light1) {
		switch (quarter) {
			case 0:
				return factor1 * dawnColor;
			case 1:
				return factor2 * dawnColor + factor1 * dayColor;
			case 2:
				return factor2 * (factor2*dayColor + factor1 * dawnColor);
			case 3:
				return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}
	} else {
		switch (quarter) {
			case 0:
				return factor2 * (factor2 * nightColor + factor1 * duskColor);
			case 1:
				return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			case 2:
				return factor1 * duskColor;
			case 3:
				return factor2 * duskColor + factor1 * nightColor;
		}
	}
	throw std::exception("The precalculated quarter is out of the expected range! This should NEVER happen.");
}
