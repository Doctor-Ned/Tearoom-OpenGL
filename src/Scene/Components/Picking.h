#ifndef PICKING_H
#define PICKING_H
#include "Component.h"
class Camera;
class Picking : public Component
{
public:
	Picking(GraphNode* _gameObject, const std::string& name, Camera* cam);
	void update(float msec) override;
	 virtual ~Picking() override;
private:
	Camera* camera;
};

#endif
