#include "Scene/Plane.h"
#include <glm/glm.hpp>

Plane::Plane(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3) : points{ point1, point2, point3 }
{
	glm::vec3 v = points[1] - points[0];
	glm::vec3 u = points[2] - points[0];

	glm::vec3 n = glm::normalize(glm::cross(v, u));

	A = n.x;
	B = n.y;
	C = n.z;

	D = A * points[0].x + B * points[0].y + C * points[0].z;
	D *= (-1.0f);
}

Plane::Plane()
{
}

float Plane::distanceToPoint(glm::vec3 point)
{
	return A * point.x + B * point.y + C * point.z + D;
}
