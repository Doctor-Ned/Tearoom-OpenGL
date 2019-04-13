#include "SphereCollider.h"
#include "Mesh/MeshColorSphere.h"
#include "Scene/GraphNode.h"

SphereCollider::SphereCollider(GraphNode* _gameObject, glm::vec3 position, float _radius) : Collider(SphereCol, _gameObject, position), radius(_radius)
{
	mesh_ptr = std::make_shared<MeshColorSphere>(radius, 15, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mesh_ptr->setUseLight(false);
	mesh_ptr->setRenderMode(GL_LINES);
}

void SphereCollider::setRadius(float size)
{
	if (size <= 0.0f)
	{
		return;
	}
	MeshColorSphere *sphere = dynamic_cast<MeshColorSphere*>(mesh_ptr.get());
	sphere->updateValues(radius, 15);
	radius = size;
}

float SphereCollider::getRadius()
{
	return radius;
}
