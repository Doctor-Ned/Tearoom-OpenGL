#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform(bool& _dirty) {
	transform = glm::mat4(1);
	dirty = &_dirty;
}

glm::mat4 Transform::Matrix()
{
	return transform;
}

void Transform::SetMatrix(const glm::mat4& matrix)
{
	transform = matrix;
	*dirty = true;
}

void Transform::Rotate(const float& angle, const glm::vec3& axis) {
	transform = glm::rotate(transform, glm::radians(angle), axis);
	*dirty = true;
}
void Transform::RotateByRadians(const float& angle, const glm::vec3& axis)
{
	transform = glm::rotate(transform, angle, axis);
	*dirty = true;
}
void Transform::Translate(const glm::vec3& translation) {
	transform = glm::translate(transform, translation);
	*dirty = true;
}
void Transform::Scale(const glm::vec3& scale) {
	transform = glm::scale(transform, scale);
	*dirty = true;
}
void Transform::setPosition(const float& x, const float& y, const float& z) {
	transform[3][0] = x;
	transform[3][1] = y;
	transform[3][2] = z;
	*dirty = true;
}

glm::vec3 Transform::getPosition()
{
	glm::vec3 position = glm::vec3(transform[3]);
	return position;
}