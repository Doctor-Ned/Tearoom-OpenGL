#ifndef TRANSFORM
#define	TRANSFORM
#include <glm/glm.hpp>

class Transform
{
private:
	bool* dirty;
	glm::mat4 transform;
public:
	Transform(bool& dirty);
	glm::mat4 getMatrix();
	void setMatrix(const glm::mat4& matrix);
	void rotate(const float& angle, const glm::vec3& axis);
	void rotateByRadians(const float& angle, const glm::vec3& axis);
	void translate(const glm::vec3& translation);
	void scale(const glm::vec3& scale);
	void setPosition(const float& x,const float& y, const float& z);
	glm::vec3 getPosition();
};
#endif