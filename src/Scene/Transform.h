#ifndef TRANSFORM
#define	TRANSFORM
#include <glm/glm.hpp>

class Transform {
public:
	Transform(bool& dirty);
	glm::mat4 getMatrix();
	glm::mat4 getUntranslatedMatrix();
	void setMatrix(const glm::mat4& matrix);
	void rotate(const float& angle, const glm::vec3& axis);
	void rotateByRadians(const float& angle, const glm::vec3& axis);
	void translate(const glm::vec3& translation);
	void scale(const glm::vec3& scale);
	void setPosition(const float& x, const float& y, const float& z);
	void setPosition(glm::vec3 pos);
	glm::vec3 getPosition();
	glm::mat4 getLastMatrix();
	void setLastMatrix(glm::mat4 mat);
protected:
	bool* dirty;
	glm::mat4 last;
	glm::mat4 transform;
	glm::mat4 untranslatedTransform;
};
#endif