#include "Transform.h"
#include "Global.h"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

Transform::Transform(bool& _dirty) {
	transform = glm::mat4(1.0f);
	last = glm::mat4(1.0f);
	dirty = &_dirty;
}

glm::mat4 Transform::getMatrix() {
	return transform;
}

void Transform::setMatrix(const glm::mat4& matrix) {
	transform = matrix;
	*dirty = true;
}

void Transform::rotate(const float& angle, const glm::vec3& axis) {
	transform = glm::rotate(transform, glm::radians(angle), axis);
	*dirty = true;
}
void Transform::rotateByRadians(const float& angle, const glm::vec3& axis) {
	transform = glm::rotate(transform, angle, axis);
	*dirty = true;
}
void Transform::translate(const glm::vec3& translation) {
	transform = glm::translate(transform, translation);
	*dirty = true;
}
void Transform::scale(const glm::vec3& scale) {
	transform = glm::scale(transform, scale);
	*dirty = true;
}
void Transform::setPosition(const float& x, const float& y, const float& z) {
	transform[3][0] = x;
	transform[3][1] = y;
	transform[3][2] = z;
	*dirty = true;
}

void Transform::setPosition(glm::vec3 pos) {
	transform[3] = glm::vec4(pos, 1.0f);
	*dirty = true;
}

void Transform::renderGui() {
	glm::mat4 matrix = getMatrix();
	glm::vec3 translation = matrix[3];
	glm::vec3 scale = Global::getScale(matrix);
	glm::vec3 euler;
	extractEulerAngleYXZ(matrix, euler.x, euler.y, euler.z);
	glm::vec3 oldTranslation = translation;
	glm::vec3 oldScale = scale;
	glm::vec3 oldEuler = euler;
	ImGui::SliderFloat3("Translation", reinterpret_cast<float*>(&translation), -10.0f, 10.0f);
	ImGui::InputFloat3("Translation (fixed)", reinterpret_cast<float*>(&translation));
	static bool uniformScale = true;
	ImGui::Checkbox("Uniform scale", &uniformScale);
	if (uniformScale) {
		float sc = glm::min(glm::min(scale.x, scale.y), scale.z);
		ImGui::SliderFloat("Scale", &sc, 0.001f, 1.0f);
		ImGui::InputFloat("Scale (fixed)", &sc);
		scale.x = sc;
		scale.y = sc;
		scale.z = sc;
	} else {
		ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&scale), 0.001f, 1.0f);
		ImGui::InputFloat3("Scale (fixed)", reinterpret_cast<float*>(&scale), 0.0001f);
	}
	//ImGui::InputFloat3("Scale (fixed)", reinterpret_cast<float*>(&scale));
	ImGui::SliderAngle("Rotation X", &euler.x);
	ImGui::SliderAngle("Rotation Y", &euler.y);
	ImGui::SliderAngle("Rotation Z", &euler.z);
	if (scale != oldScale) {
		if (scale.x <= 0.0f || scale.y <= 0.0f || scale.z <= 0.0f) {
			//some error or something? we might as well just leave it.
		} else {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					if (oldScale[i] != 0.0f) {
						matrix[i][j] /= oldScale[i];
					}
					matrix[i][j] *= scale[i];
				}
			}
		}
		transform = matrix;
		*dirty = true;
	}
	if (euler != oldEuler) {
		glm::mat4 rotation = glm::eulerAngleYXZ(euler.x, euler.y, euler.z);
		for(int i=0;i<3;i++) {
			for(int j=0;j<3;j++) {
				matrix[i][j] = i==j ? scale[i] : 0.0f;
			}
		}
		glm::vec3 pos = matrix[3];
		matrix[3].x = 0.0f;
		matrix[3].y = 0.0f;
		matrix[3].z = 0.0f;
		//matrix = glm::rotate(matrix, euler.z, glm::vec3(0.0f, 0.0f, 1.0f));
		//matrix = glm::rotate(matrix, euler.x, glm::vec3(1.0f, 0.0f, 0.0f));
		//matrix = glm::rotate(matrix, euler.y, glm::vec3(0.0f, 1.0f, 0.0f));
		matrix = rotation * matrix;
		matrix[3].x = pos.x;
		matrix[3].y = pos.y;
		matrix[3].z = pos.z;
		transform = matrix;
		*dirty = true;
	}

	if (translation != oldTranslation) {
		setPosition(translation);
	}
	/*glm::mat4 rot = glm::eulerAngleXYZ(euler.x,euler.y, euler.z);
	matrix = glm::mat4(1.0f);
	matrix = glm::translate(matrix, translation);
	matrix = rot * matrix;
	setMatrix(matrix);*/
	//printf("przed: %f|%f|%f|%f, po: %f|%f|%f|%f\n", rotation.x, rotation.y, rotation.z, rotation.w, back.x, back.y, back.z, back.w);
	//printf("euler: %f|%f|%f\n", euler.x, euler.y, euler.z);
	ImGui::NewLine();
}

glm::vec3 Transform::getPosition() {
	glm::vec3 position = glm::vec3(transform[3]);
	return position;
}

glm::mat4 Transform::getLastMatrix() {
	return last;
}

void Transform::setLastMatrix(glm::mat4 mat) {
	last = mat;
}
