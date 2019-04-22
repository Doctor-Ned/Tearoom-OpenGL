#ifndef PICKING_H
#define PICKING_H
#include "Component.h"
class Camera;
class Picking : public Component
{
public:
	Picking(GraphNode* _gameObject, const std::string& name, Camera* cam);
	void update(float msec) override;
	 ~Picking() override;
	 SerializableType getSerializableType() override;
	 Json::Value serialize(Serializer *serializer) override;
	 void deserialize(Json::Value &root, Serializer* serializer) override;
private:
	Camera* camera;
	Picking(){}
	friend class Serializer;
};

#endif
