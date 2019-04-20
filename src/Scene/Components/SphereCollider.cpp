#include "SphereCollider.h"
#include "Mesh/MeshColorSphere.h"
#include "Scene/GraphNode.h"

SphereCollider::SphereCollider(GraphNode* _gameObject,  Collision classification, bool isTrigger, glm::vec3 position, float _radius) : Collider(SphereCol, _gameObject, classification, isTrigger,  position), radius(_radius)
{
	name = "Sphere collider";
	mesh_ptr = std::make_shared<MeshColorSphere>(radius, 15, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	mesh_ptr->setUseLight(false);
	//mesh_ptr->setRenderMode(GL_LINE_STRIP);
	mesh_ptr->setOpacity(0.2f);
}

void SphereCollider::setRadius(float size)
{
	if (size <= 0.0f)
	{
		return;
	}
	MeshColorSphere *sphere = dynamic_cast<MeshColorSphere*>(mesh_ptr.get());
	radius = size;
	sphere->updateValues(radius, 15);
}

float SphereCollider::getRadius()
{
	return radius;
}
