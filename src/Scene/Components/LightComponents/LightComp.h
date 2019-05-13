#pragma once

#include "Scene/Components/Component.h"
#include "Global.h"

class LightComp : public Component {
public:
	LightComp(GraphNode *gameObject);
	void updateWorld() override;
	void setComponentActive(bool active) override;
	virtual glm::vec4 getColor() = 0;
	virtual float getNearPlane() = 0;
	virtual float getFarPlane() = 0;
	virtual void setColor(glm::vec4 color);
	SerializableType getSerializableType() override;
	virtual void setModel(glm::mat4 model) = 0;
	virtual void setNearPlane(float nearPlane) = 0;
	virtual void setFarPlane(float farPlane) = 0;
	bool getOverrideMeshColor();
	void setOverrideMeshColor(bool overrideColor);
protected:
	bool overrideMeshColor = false;
	void setMeshColor(glm::vec4 color);
	virtual bool getEnabled() = 0;
	virtual void setEnabled(bool enabled) = 0;
	void renderGui() override;
	LightComp(){}
};
