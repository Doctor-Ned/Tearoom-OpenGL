#ifndef CAMERA_H
#define CAMERA_H

#include "Global.h"
#include "Scene/Components/Collider.h"
#include "Scene/Frustum.h"


class GraphNode;
class Camera final : public Component {
public:
	Camera(GraphNode *gameObject);
	glm::mat4 getView();
	glm::mat4 getUntranslatedView();
	void updateWorld() override;
	Frustum getFrustum();
	GraphNode* castRayFromCamera(glm::vec3 direction, float distance, Collider* toIgnore = nullptr);
	void RecalculateFrustum();
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	Camera() {}
	glm::mat4 view;
	Frustum frustum;
	friend class Serializer;
};

#endif
