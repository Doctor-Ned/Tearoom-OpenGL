#pragma once

#include "LightNode.h"

class QuadraticLightNode : public LightNode {
public:
	QuadraticLightNode(Mesh *mesh, GraphNode *parent) : LightNode(mesh,parent){}
	virtual float getConstant() = 0;
	virtual float getLinear() = 0;
	virtual float getQuadratic() = 0;
	virtual void setConstant(float constant) = 0;
	virtual void setLinear(float linear) = 0;
	virtual void setQuadratic(float quadratic) = 0;
protected:
	void renderGui() override;
};