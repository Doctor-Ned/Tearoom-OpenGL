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

void BoxCollider::setHalfDimensions(glm::vec3& half_dimensions)
{
	this->halfDimensions = half_dimensions;
	MeshColorBox *box = dynamic_cast<MeshColorBox*>(mesh_ptr.get());
	box->updateValues(glm::vec3(-halfDimensions.x, -halfDimensions.y, -halfDimensions.z), glm::vec3(halfDimensions.x, halfDimensions.y, halfDimensions.z));
}

glm::vec3 BoxCollider::getHalfDimensions()
{
	return halfDimensions;
}

void BoxCollider::renderGui()
{
	Collider::renderGui();

	ImGui::Text("HalfDimensions:");
	glm::vec3 hDim = halfDimensions;
	
	ImGui::DragFloat("HalfX: ", &hDim.x, 0.005f);
	ImGui::DragFloat("HalfY: ", &hDim.y, 0.005f);
	ImGui::DragFloat("HalfZ: ", &hDim.z, 0.005f);
	glm::vec3 hDim2;
	for (int i = 0; i < 3; i++)
	{
		(hDim[i] < 0.0f) ? (hDim2[i] = 0.0f) : (hDim2[i] = hDim[i]);
	}
	setHalfDimensions(hDim2);
}
