#pragma once

#include "GraphNode.h"

class Camera;

class BillboardNode : public GraphNode {
public:
	BillboardNode(Camera *camera, Mesh* mesh = nullptr, GraphNode* parent = nullptr, bool rescale = true);
	void setCamera(Camera *camera);
	void update(double timeDiff) override;
	Camera *getCamera();
	void setRescaling(bool rescale);
	bool getRescaling();
protected:
	void recalculateMatrix();
	Camera *camera;
	bool rescale;
	void updateWorld() override;
};
