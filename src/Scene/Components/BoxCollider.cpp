#include "BoxCollider.h"
#include "Mesh/MeshColorBox.h"

BoxCollider::BoxCollider(GraphNode* _gameObject, glm::vec3 position, glm::vec3 half_dimensions) : Collider(BoxCol, _gameObject, position), halfDimensions(half_dimensions)
{
	mesh_ptr = std::make_shared<MeshColorBox>(glm::vec3(-halfDimensions.x, -halfDimensions.y, -halfDimensions.z), glm::vec3(halfDimensions.x, halfDimensions.y, halfDimensions.z), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mesh_ptr->setUseLight(false);
	mesh_ptr->setRenderMode(GL_LINES);

	mat = glm::mat4(1);
}

void BoxCollider::setHalfDimensions(glm::vec3 half_dimensions)
{
	mesh_ptr = std::make_shared<MeshColorBox>(glm::vec3(-halfDimensions.x, -halfDimensions.y, -halfDimensions.z), glm::vec3(halfDimensions.x, halfDimensions.y, halfDimensions.z), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mesh_ptr->setUseLight(false);
	mesh_ptr->setRenderMode(GL_LINES);
}

glm::vec3 BoxCollider::getHalfDimensions()
{
	return halfDimensions;
}
