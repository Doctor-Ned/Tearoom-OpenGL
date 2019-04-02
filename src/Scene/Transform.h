#ifndef TRANSFORM
#define	TRANSFORM
#include <glm/glm.hpp>

class Transform
{
private:
	bool* dirty;
public:
	glm::mat4 transform; //shh, that's just for now
	Transform(bool& dirty);
	glm::mat4 Matrix();
	void SetMatrix(const glm::mat4& matrix);
	void Rotate(const float& angle, const glm::vec3& axis);
	void RotateByRadians(const float& angle, const glm::vec3& axis);
	void Translate(const glm::vec3& translation);
	void Scale(const glm::vec3& scale);
	void setPosition(const float& x,const float& y, const float& z);
	glm::vec3 getPosition();
};
#endif