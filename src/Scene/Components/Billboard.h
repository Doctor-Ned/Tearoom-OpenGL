#pragma once

#include "Scene/GraphNode.h"
#include "Scene/AssetManager.h"

class Camera;

class Billboard : public Component {
public:
	Billboard(Camera *camera, GraphNode* node, bool rescale = true);
	void setCamera(Camera *camera);
	void update(float msec) override;
	void updateWorld() override;
	Camera *getCamera();
	void setRescaling(bool rescale);
	bool getRescaling();
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	void renderGui() override;
	void recalculateMatrix();
	Camera *camera;
	bool rescale;
	Billboard(){}
	friend class Serializer;
};
