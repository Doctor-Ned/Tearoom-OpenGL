#include "Global.h"
#include "ComposedTransform.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/euler_angles.inl>

ComposedTransform::ComposedTransform(bool& dirty) {
	this->dirty = &dirty;
	dirty = selfDirty = true;
	updateLast();
}

void ComposedTransform::recalculateMatrix() {
	transform = dataToMatrix(data);
}

glm::mat4 ComposedTransform::getMatrix() {
	if (selfDirty) {
		recalculateMatrix();
	}
	return transform;
}

glm::mat4 ComposedTransform::getLastMatrix() const {
	return lastTransform;
}

glm::vec3 ComposedTransform::getPosition() const {
	return data.translation;
}

void ComposedTransform::initialize(glm::vec3 position, glm::vec3 eulerRotation) {
	initialize(position, glm::vec3(1.0f, 1.0f, 1.0f), eulerRotation);
}

void ComposedTransform::initialize(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation) {
	data.translation = position;
	data.scale = scale;
	data.eulerRotation = eulerRotation;
	*dirty = true;
	selfDirty = true;
}

void ComposedTransform::initialize(float x, float y, float z, float scale, float rotX, float rotY, float rotZ) {
	initialize(glm::vec3(x, y, z), glm::vec3(scale, scale, scale), glm::vec3(rotX, rotY, rotZ));
}

void ComposedTransform::initializeDegrees(glm::vec3 position, glm::vec3 eulerRotation) {
	initialize(position, Global::degreesToRadians(eulerRotation));
}

void ComposedTransform::initializeDegrees(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation) {
	initialize(position, scale, Global::degreesToRadians(eulerRotation));
}

void ComposedTransform::initializeDegrees(float x, float y, float z, float scale, float rotX, float rotY, float rotZ) {
	initialize(glm::vec3(x, y, z), glm::vec3(scale, scale, scale), Global::degreesToRadians(glm::vec3(rotX, rotY, rotZ)));
}

void ComposedTransform::setPosition(glm::vec3 position) {
	data.translation = position;
	*dirty = true;
	selfDirty = true;
}

void ComposedTransform::setPosition(float x, float y, float z) {
	setPosition(glm::vec3(x, y, z));
}

void ComposedTransform::translate(glm::vec3 translation) {
	data.translation += translation;
	*dirty = true;
	selfDirty = true;
}

void ComposedTransform::translate(float x, float y, float z) {
	translate(glm::vec3(x, y, z));
}

void ComposedTransform::setScale(glm::vec3 scale) {
	data.scale = scale;
	*dirty = true;
	selfDirty = true;
}

void ComposedTransform::setScale(float scale) {
	setScale(glm::vec3(scale, scale, scale));
}

void ComposedTransform::setRotation(glm::vec3 euler) {
	data.eulerRotation = euler;
	*dirty = true;
	selfDirty = true;
}

void ComposedTransform::setRotationDegrees(glm::vec3 euler) {
	setRotation(Global::degreesToRadians(euler));
}

void ComposedTransform::setRotationX(float x) {
	data.eulerRotation.x = x;
	*dirty = true;
	selfDirty = true;
}

void ComposedTransform::setRotationXDegrees(float x) {
	setRotationX(glm::radians(x));
}

void ComposedTransform::setRotationY(float y) {
	data.eulerRotation.y = y;
	*dirty = true;
	selfDirty = true;
}

void ComposedTransform::setRotationYDegrees(float y) {
	setRotationY(glm::radians(y));
}

void ComposedTransform::setRotationZ(float z) {
	data.eulerRotation.z = z;
	*dirty = true;
	selfDirty = true;
}

void ComposedTransform::setRotationZDegrees(float z) {
	setRotationZ(glm::radians(z));
}

void ComposedTransform::rotateX(float x) {
	data.eulerRotation.x += x;
	*dirty = true;
	selfDirty = true;
}

void ComposedTransform::rotateXDegrees(float x) {
	rotateX(glm::radians(x));
}

void ComposedTransform::rotateY(float y) {
	data.eulerRotation.y += y;
	*dirty = true;
	selfDirty = true;
}

void ComposedTransform::rotateYDegrees(float y) {
	rotateY(glm::radians(y));
}

void ComposedTransform::rotateZ(float z) {
	data.eulerRotation.z += z;
	*dirty = true;
	selfDirty = true;
}

void ComposedTransform::rotateZDegrees(float z) {
	rotateZ(glm::radians(z));
}

void ComposedTransform::updateLast() {
	lastData = data;
	lastTransform = dataToMatrix(lastData);
}

void ComposedTransform::setData(const TransformData data) {
	this->data = data;
	*dirty = true;
	selfDirty = true;
}

void ComposedTransform::setLastData(const TransformData data) {
	this->lastData = data;
	lastTransform = dataToMatrix(lastData);
}

TransformData ComposedTransform::getData() const {
	return data;
}

TransformData ComposedTransform::getLastData() const {
	return lastData;
}

glm::mat4 ComposedTransform::dataToMatrix(const TransformData data) {
	return standardDataToMatrix(data);
}

glm::mat4 ComposedTransform::standardDataToMatrix(TransformData data) {
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), data.translation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), data.scale);
	glm::mat4 rotation = glm::eulerAngleYXZ(data.eulerRotation.y, data.eulerRotation.x, data.eulerRotation.z);
	return translation * rotation * scale;
}

void ComposedTransform::renderGui() {
	glm::vec3 translation = data.translation, scale = data.scale, eulerRotation = data.eulerRotation;
	ImGui::DragFloat3("Translation", reinterpret_cast<float*>(&translation), 0.1f);
	ImGui::InputFloat3("Translation (fixed)", reinterpret_cast<float*>(&translation));
	static bool uniformScale = true;
	ImGui::Checkbox("Uniform scale", &uniformScale);
	if (uniformScale) {
		float scl = glm::min(glm::min(scale.x, scale.y), scale.z);
		ImGui::DragFloat("Scale", &scl, 0.1f);
		ImGui::InputFloat("Scale (fixed)", &scl);
		scale.x = scl;
		scale.y = scl;
		scale.z = scl;
	} else {
		ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&scale), 0.1f);
		ImGui::InputFloat3("Scale (fixed)", reinterpret_cast<float*>(&scale));
	}
	glm::vec3 eulerRotationDegrees = Global::radiansToDegrees(eulerRotation);
	ImGui::SliderAngle("X Rotation", &eulerRotation.x);
	ImGui::InputFloat("X Rotation (fixed)", &eulerRotationDegrees.x);
	ImGui::SliderAngle("Y Rotation", &eulerRotation.y);
	ImGui::InputFloat("Y Rotation (fixed)", &eulerRotationDegrees.y);
	ImGui::SliderAngle("Z Rotation", &eulerRotation.z);
	ImGui::InputFloat("Z Rotation (fixed)", &eulerRotationDegrees.z);
	if(translation != data.translation || scale != data.scale || eulerRotation != data.eulerRotation || Global::degreesToRadians(eulerRotationDegrees) != data.eulerRotation) {
		glm::vec3 rot;
		if(eulerRotation != data.eulerRotation) {
			rot = eulerRotation;
		} else {
			rot = Global::degreesToRadians(eulerRotationDegrees);
		}
		initialize(translation, scale, rot);
	}
}
