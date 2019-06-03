#include "Collider.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/GraphNode.h"
#include "Serialization/Serializer.h"
#include "Serialization/DataSerializer.h"
#include <iostream>

void Collider::update(float m_sec) {
}

void Collider::updateDrawData() {
	matrix = translate(glm::mat4(1.0f), glm::vec3(gameObject->worldTransform.getMatrix() * translate(glm::mat4(1.0f), positionOffset)[3]));
}

void Collider::drawSelf(Shader* shader) {
	//mesh_ptr->drawSelf(shader, matrix);
}

ShaderType Collider::getShaderType() {
	return mesh_ptr->getShaderType();
}

void Collider::setCollisionCallback(std::function<void(Collider*)> f) {
	callbackFunctions.push_back(f);
}

void Collider::renderGui()
{
	Component::renderGui();
	bool isDynamic = getCollisionType() == DYNAMIC;

	ImGui::Checkbox("IsTrigger", &isTrigger);
	ImGui::Checkbox("IsDynamic", &isDynamic);

	//if(getCollisionType() != (isDynamic ? DYNAMIC : STATIC)) {
		collisionType = isDynamic ? DYNAMIC : STATIC;
	//}

	ImGui::Text("Offset:");
	ImGui::DragFloat("X: ", &positionOffset.x, 0.005f);
	ImGui::DragFloat("Y: ", &positionOffset.y, 0.005f);
	ImGui::DragFloat("Z: ", &positionOffset.z, 0.005f);

	ImGui::InputFloat("fixed X:", &positionOffset.x);
	ImGui::InputFloat("fixed Y:", &positionOffset.y);
	ImGui::InputFloat("fixed Z:", &positionOffset.z);

	ImGui::NewLine();
}

SerializableType Collider::getSerializableType() {
	return SCollider;
}

Json::Value Collider::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["mesh"] = serializer->serialize(mesh_ptr.get());
	root["matrix"] = DataSerializer::serializeMat4(matrix);
	root["collisionType"] = static_cast<int>(collisionType);
	root["type"] = static_cast<int>(type);
	root["isTrigger"] = isTrigger;
	root["positionOffset"] = DataSerializer::serializeVec3(positionOffset);
	return root;
}

void Collider::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	Mesh *mesh = dynamic_cast<Mesh*>(serializer->deserialize(root["mesh"]).object);
	if(mesh == nullptr) {
		mesh_ptr = nullptr;
	} else {
		mesh_ptr = std::shared_ptr<Mesh>(mesh);
	}
	matrix = DataSerializer::deserializeMat4(root["matrix"]);
	collisionType = static_cast<Collision>(root["collisionType"].asInt());
	type = static_cast<ColliderType>(root["type"].asInt());
	isTrigger = root["isTrigger"].asBool();
	positionOffset = DataSerializer::deserializeVec3(root["positionOffset"]);
}

bool Collider::isActive() {
	return gameObject->isActive();
}

Collider::Collider(ColliderType _type, GraphNode* _gameObject, Collision classification, bool _isTrigger, glm::vec3 positionOffset)
	: Component(_gameObject), type(_type), collisionType(classification), isTrigger(_isTrigger), positionOffset(positionOffset) { }

bool Collider::isOpaque() {
	return mesh_ptr != nullptr && mesh_ptr->isOpaque();
}

Collider::~Collider() {

}

glm::vec3 Collider::getPosition() {
	return matrix[3];
}

ColliderType Collider::getType() {
	return type;
}

Collision Collider::getCollisionType()
{
	return collisionType;
}

bool Collider::getIsTrigger()
{
	return isTrigger;
}

void Collider::setMatrix(glm::mat4 mat)
{
	matrix = mat;
}

void Collider::setIsTrigger(bool _isTrigger)
{
	isTrigger = _isTrigger;
}

std::vector<std::function<void(Collider*)>> Collider::getCallbackFunctions() {
	return callbackFunctions;
}

void Collider::setPositionOffset(glm::vec3 positionOffset) {
	this->positionOffset = positionOffset;
}
