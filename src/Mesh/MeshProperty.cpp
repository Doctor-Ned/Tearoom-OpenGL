#include "MeshProperty.h"

void MeshProperty::draw(Shader *shader, glm::mat4 world) {
	Mesh::draw(shader, world);
	shader->setRoughness(roughness);
	shader->setMetallic(metallic);
	shader->setEmissive(emissive);
	shader->setBool("moving", moving);
	if(moving)
	{
		shader->setFloat("time", glfwGetTime());
		shader->setFloat("speed", speed);
		shader->setFloat("moveScale", moveScale);
	}
}

MeshProperty::MeshProperty(ShaderType shaderType) : Mesh(shaderType) { }

void MeshProperty::setRoughness(float roughness) {
	this->roughness = roughness;
}

void MeshProperty::setMetallic(float metallic) {
	this->metallic = metallic;
}

void MeshProperty::setEmissive(glm::vec3 emissive) {
	this->emissive = emissive;
}

float MeshProperty::getRoughness() const {
	return roughness;
}

float MeshProperty::getMetallic() const {
	return metallic;
}

glm::vec3 MeshProperty::getEmissive() const {
	return emissive;
}

Json::Value MeshProperty::serialize(Serializer* serializer) {
	Json::Value root = Mesh::serialize(serializer);
	root["roughness"] = roughness;
	root["metallic"] = metallic;
	root["emissive"] = DataSerializer::serializeVec3(emissive);
	root["speed"] = speed;
	root["moveScale"] = moveScale;
	root["moving"] = moving;
	return root;
}

void MeshProperty::deserialize(Json::Value& root, Serializer* serializer) {
	Mesh::deserialize(root, serializer);
	setRoughness(root.get("roughness", roughness).asFloat());
	setMetallic(root.get("metallic", metallic).asFloat());
	setEmissive(DataSerializer::deserializeVec3(root.get("emissive", DataSerializer::serializeVec3(emissive))));
	speed = root.get("speed", speed).asFloat();
	moveScale = root.get("moveScale", moveScale).asFloat();
	moving = root.get("moving", moving).asBool();
}

SerializableType MeshProperty::getSerializableType() {
	return SMeshProperty;
}

void MeshProperty::renderGui() {
	Mesh::renderGui();
	float roughness = this->roughness, metallic = this->metallic;
	float speed = this->speed, moveScale = this->moveScale;
	bool moving = this->moving;
	glm::vec3 emissive = this->emissive;
	ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
	ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f);
	ImGui::ColorEdit3("Emissive", reinterpret_cast<float*>(&emissive));
	ImGui::Checkbox("moving", &moving);
	ImGui::InputFloat("speed", &speed, 0.005f, 0.1f);
	ImGui::InputFloat("moveScale", &moveScale, 0.005f, 0.1f);
	if(roughness != this->roughness) {
		setRoughness(roughness);
	}
	if(metallic != this->metallic) {
		setMetallic(metallic);
	}
	if(emissive != this->emissive) {
		setEmissive(emissive);
	}
	if(speed != this->speed)
	{
		this->speed = speed;
	}
	if(moveScale != this->moveScale)
	{
		this->moveScale = moveScale;
	}
	if(moving != this->moving)
	{
		this->moving = moving;
	}
}
