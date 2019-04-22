#include "BoxCollider.h"
#include "Mesh/MeshColorBox.h"
#include "Serialization/DataSerializer.h"

SerializableType BoxCollider::getSerializableType() {
	return SBoxCollider;
}

Json::Value BoxCollider::serialize(Serializer* serializer) {
	Json::Value root = Collider::serialize(serializer);
	root["halfDimensions"] = DataSerializer::serializeVec3(halfDimensions);
	return root;
}

void BoxCollider::deserialize(Json::Value& root, Serializer* serializer) {
	Collider::deserialize(root, serializer);
	setHalfDimensions(DataSerializer::deserializeVec3(root["halfDimensions"]));
}

BoxCollider::BoxCollider(GraphNode* _gameObject, Collision classification , bool isTrigger, glm::vec3 positionOffset, glm::vec3 halfDims) : Collider(BoxCol, _gameObject, classification, isTrigger, positionOffset), halfDimensions(halfDims)
{
	name = "Box collider";
	mesh_ptr = std::make_shared<MeshColorBox>(glm::vec3(-halfDimensions.x, -halfDimensions.y, -halfDimensions.z), glm::vec3(halfDimensions.x, halfDimensions.y, halfDimensions.z), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mesh_ptr->setUseLight(false);
	mesh_ptr->setRenderMode(GL_LINE_STRIP);
}

void BoxCollider::setHalfDimensions(glm::vec3 half_dimensions)
{
	this->halfDimensions = half_dimensions;
	MeshColorBox *box = dynamic_cast<MeshColorBox*>(mesh_ptr.get());
	box->updateValues(glm::vec3(-halfDimensions.x, -halfDimensions.y, -halfDimensions.z), glm::vec3(halfDimensions.x, halfDimensions.y, halfDimensions.z));
}

glm::vec3 BoxCollider::getHalfDimensions()
{
	return halfDimensions;
}
