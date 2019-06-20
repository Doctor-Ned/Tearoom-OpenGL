#ifndef FRUSTUM_H
#define FRUSTUM_H
#include "glm/glm.hpp"
#include "Scene/Plane.h"
#include "Components/SphereCollider.h"
#include "Components/BoxCollider.h"

struct Box;
class Frustum
{
public:
	glm::vec3 nearTopLeft;
	glm::vec3 nearTopRight;
	glm::vec3 nearBottomLeft;
	glm::vec3 nearBottomRight;
	glm::vec3 farTopLeft;
	glm::vec3 farTopRight;
	glm::vec3 farBottomLeft;
	glm::vec3 farBottomRight;
	Plane planes[6];
	bool pointInFrustum(glm::vec3 point);
	bool sphereInFrustum(SphereCollider* sphere);
	bool boxInFrustum(BoxCollider* box);
	bool boxInFrustum(const Box& box);
};
#endif

