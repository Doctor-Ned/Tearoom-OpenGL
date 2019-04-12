#ifndef ROTATINGNODE_H
#define ROTATINGNODE_H

#include "GraphNode.h"

class RotatingNode : public GraphNode {
public:
	RotatingNode(float rotationSpeed, Mesh* mesh = nullptr, GraphNode* parent = nullptr);
	void update(double timeDiff) override;
	void draw() { GraphNode::updateDrawData(); };
	virtual void setRotationSpeed(float speed);
protected:
	float rotationSpeed;
};

#endif
