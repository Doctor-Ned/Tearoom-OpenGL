#ifndef PLANE_H
#define PLANE_H
#include <glm/vec3.hpp>

enum Planes
{
	TOP = 0, BOTTOM, LEFT,
	RIGHT, NEARP, FARP
};

class Plane
{
private:
	glm::vec3 points[3];
	float A, B, C, D;
public:
	Plane(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3);
	Plane();
	float distanceToPoint(glm::vec3 point);
};
#endif