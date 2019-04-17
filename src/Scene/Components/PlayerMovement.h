#ifndef PLAYER_MOVEMENT_H
#define PLAYER_MOVEMENT_H
#include "Scene/Components/Component.h"

class Camera;
class Scene;
class PlayerMovement : public Component
{
public:
	Camera* camera;
	Scene* scene;
	PlayerMovement(GraphNode* _gameObject, Camera* _camera, Scene* _scene);

	void update(float msec) override;
};
#endif