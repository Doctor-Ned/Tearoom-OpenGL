#pragma once
#include <glm/glm.hpp>
#include "GuiConfigurable.h"
#include <glm/gtc/quaternion.hpp>

struct TransformData {
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 eulerRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat quatRotation = glm::quat();
};

class ComposedTransform : public GuiConfigurable {
public:
	ComposedTransform(bool& dirty);
	void recalculateMatrix();
	void recalculateMatrixQuat();
	glm::mat4 getMatrix();
	glm::mat4 getMatrixQuat();
	glm::mat4 getLastMatrix() const;
	glm::vec3 getPosition() const;
	glm::quat getQuaternion();
	void initialize(glm::vec3 position, glm::vec3 eulerRotation);
	void initialize(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation);
	void initialize(float x, float y, float z, float scale = 1.0f, float rotX = 0.0f, float rotY = 0.0f, float rotZ = 0.0f);
	void initializeDegrees(glm::vec3 position, glm::vec3 eulerRotation);
	void initializeDegrees(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation);
	void initializeDegrees(float x, float y, float z, float scale = 1.0f, float rotX = 0.0f, float rotY = 0.0f, float rotZ = 0.0f);
	void setPosition(glm::vec3 position);
	void setPosition(float x, float y, float z);
	void translate(glm::vec3 translation);
	void translate(float x, float y, float z);
	void setScale(glm::vec3 scale);
	void setScale(float scale);
	void setQuatRotation(glm::quat q);
	void setRotation(glm::vec3 euler);
	void setRotationDegrees(glm::vec3 euler);
	void setRotationX(float x);
	void setRotationXDegrees(float x);
	void setRotationY(float y);
	void setRotationYDegrees(float y);
	void setRotationZ(float z);
	void setRotationZDegrees(float z);
	void rotateX(float x);
	void rotateXDegrees(float x);
	void rotateY(float y);
	void rotateYDegrees(float y);
	void rotateZ(float z);
	void rotateZDegrees(float z);
	void updateLast();
	void setData(TransformData data);
	void setLastData(TransformData data);
	TransformData getData() const;
	TransformData getLastData() const;
	virtual glm::mat4 dataToMatrix(TransformData data);
	virtual glm::mat4 dataToMatrixQuat(TransformData data);
	static glm::mat4 standardDataToMatrix(TransformData data);
	static glm::mat4 standardDataToMatrixQuat(TransformData data);
protected:
	TransformData data, lastData;
	glm::mat4 transform, lastTransform;
	bool* dirty;
	bool selfDirty;
	void renderGui() override;
};