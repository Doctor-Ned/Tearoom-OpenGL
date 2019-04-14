#include "LightNode.h"

void LightNode::setActive(bool active) {
	if (active != this->active) {
		if (active) {
			setLightAmbient(lastAmbient);
			setLightDiffuse(lastDiffuse);
			setLightSpecular(lastSpecular);
		} else {
			lastAmbient = getLightAmbient();
			lastDiffuse = getLightDiffuse();
			lastSpecular = getLightSpecular();
			setLightAmbient(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
			setLightDiffuse(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
			setLightSpecular(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		}
	}
	GraphNode::setActive(active);
}

glm::vec4 LightNode::getAmbient() {
	return getAmbient(false);
}

glm::vec4 LightNode::getDiffuse() {
	return getDiffuse(false);
}

glm::vec4 LightNode::getSpecular() {
	return getSpecular(false);
}

void LightNode::setAmbient(glm::vec4 ambient) {
	setAmbient(ambient, false);
}

void LightNode::setDiffuse(glm::vec4 diffuse) {
	setDiffuse(diffuse, false);
}

void LightNode::setSpecular(glm::vec4 specular) {
	setSpecular(specular, false);
}

void LightNode::setAmbient(glm::vec4 ambient, bool force) {
	if(force || active) {
		setLightAmbient(ambient);
	} else {
		lastAmbient = ambient;
	}
}

void LightNode::setDiffuse(glm::vec4 diffuse, bool force) {
	if(force || active) {
		setLightDiffuse(diffuse);
	} else {
		lastDiffuse = diffuse;
	}
}

void LightNode::setSpecular(glm::vec4 specular, bool force) {
	if(force || active) {
		setLightSpecular(specular);
	} else {
		lastSpecular = specular;
	}
}

glm::vec4 LightNode::getAmbient(bool force) {
	return force || active ? getLightAmbient() : lastAmbient;
}

glm::vec4 LightNode::getDiffuse(bool force) {
	return force || active ? getLightDiffuse() : lastDiffuse;
}

glm::vec4 LightNode::getSpecular(bool force) {
	return force || active ? getLightSpecular() : lastSpecular;
}

void LightNode::updateWorld() {
	GraphNode::updateWorld();
	//if (dirty) {
	setModel(worldTransform.getMatrix());
//}
}

void LightNode::renderGui() {
	GraphNode::renderGui();
	if (active) {
		glm::vec4 ambient = getAmbient(), diffuse = getDiffuse(), specular = getSpecular();
		ImGui::ColorEdit4("Ambient", &ambient[0]);
		ImGui::NewLine();
		ImGui::ColorEdit4("Diffuse", &diffuse[0]);
		ImGui::NewLine();
		ImGui::ColorEdit4("Specular", &specular[0]);
		ImGui::NewLine();
		setAmbient(ambient);
		setDiffuse(diffuse);
		setSpecular(specular);
	}
}
