#include "Scene/Frustum.h"

bool Frustum::pointInFrustum(glm::vec3 point)
{
	for (int i = 0; i < 6; i++)
	{
		if (planes[i].distanceToPoint(point) < 0)
			return false;
	}
	return true;
}

bool Frustum::sphereInFrustum(SphereCollider* sphere)
{
	bool result = true;
	float distance;
	if (sphere != nullptr)
	{
		for (int i = 0; i < 6; i++) {
			distance = planes[i].distanceToPoint(sphere->getPosition());
			if (distance < -sphere->getRadius())
				result = false;
			else if (distance < sphere->getRadius())
				result = true;
		}
	}
	return result;
}

bool Frustum::boxInFrustum(BoxCollider* box)
{
	int result = 1, out = 0, in = 0;
	glm::vec3 halfDimensions = box->getHalfDimensions();
	glm::vec3 minPos = box->getPosition() - halfDimensions;
	glm::vec3 maxPos = box->getPosition() + halfDimensions;
	glm::vec3 boxVertices[8] = {
		minPos,
		maxPos,
		minPos + glm::vec3(2 * halfDimensions.x, 0.0f, 0.0f),
		minPos + glm::vec3(0.0f, 2 * halfDimensions.y, 0.0f),
		minPos + glm::vec3(0.0f, 0.0f, 2 * halfDimensions.z),
		maxPos - glm::vec3(2 * halfDimensions.x, 0.0f, 0.0f),
		maxPos - glm::vec3(0.0f, 2 * halfDimensions.y, 0.0f),
		maxPos - glm::vec3(0.0f, 0.0f, 2 * halfDimensions.z)
	};
	// for each plane do ...
	for (int i = 0; i < 6; i++) {

		// reset counters for corners in and out
		out = 0; in = 0;
		// for each corner of the box do ...
		// get out of the cycle as soon as a box as corners
		// both inside and out of the frustum
		for (int k = 0; k < 8 && (in == 0 || out == 0); k++) {

			// is the corner outside or inside
			if (planes[i].distanceToPoint(boxVertices[k]) < 0)
				out++;
			else
				in++;
		}
		//if all corners are out
		if (!in)
			return false;
		// if some corners are out and others are in
		if (out)
			result = 1;
	}
	return (bool)result;
}