#pragma once

#include "Scene/Components/Component.h"
#include "Global.h"

class LightComp : public Component {
public:
	LightComp(GraphNode *gameObject);
	void updateWorld() override;
	void setComponentActive(bool active) override;
	virtual glm::vec4 getAmbient() = 0;
	virtual glm::vec4 getDiffuse() = 0;
	virtual glm::vec4 getSpecular() = 0;
	virtual void setAmbient(glm::vec4 ambient) = 0;
	virtual void setDiffuse(glm::vec4 diffuse) = 0;
	virtual void setSpecular(glm::vec4 specular) = 0;
protected:
	virtual bool getEnabled() = 0;
	virtual void setEnabled(bool enabled) = 0;
	virtual void setModel(glm::mat4 model) = 0;
	void renderGui() override;
};
