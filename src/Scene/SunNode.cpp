#include "SunNode.h"
#include "Render/LightManager.h"
#include "Mesh/MeshColorSphere.h"

SunNode::SunNode(DirLight* light1, DirLight* light2, glm::vec4 dawnColor, glm::vec4 dayColor, glm::vec4 duskColor,
	glm::vec4 nightColor, float sunDistance, float initialTime, GraphNode *parent) : GraphNode(nullptr, parent), light1(light1), light2(light2),
	dawnColor(dawnColor), dayColor(dayColor), duskColor(duskColor), nightColor(nightColor), sunDistance(sunDistance) {
	setTime(initialTime);
	light1Node = new DirLightNode(light1, new MeshColorSphere(0.25f, 30, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)), this);
	light1Node->getMesh()->setShaderType(STLight);
	light2Node = new DirLightNode(light2, new MeshColorSphere(0.25f, 30, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)), this);
	light2Node->getMesh()->setShaderType(STLight);
	light1Node->localTransform.setMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -sunDistance)));
	light1Node->localTransform.rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	light2Node->localTransform.setMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, sunDistance)));
}

void SunNode::setTime(float time) {
	this->time = time;
	dirty = true;
}

void SunNode::addTime(float time) {
	this->time = this->time + time;
	dirty = true;
}

void SunNode::setVerticalRotation(float angle) {
	this->rotationAngle = angle;
	dirty = true;
}

float SunNode::getTime() {
	return time;
}

float SunNode::rescaleTime(float time) {
	while (time >= 24.0f) {
		time -= 24.0f;
	}
	while (time < 0.0f) {
		time += 24.0f;
	}
	return time;
}

glm::vec4 SunNode::timeToColor(float time, bool light1) {
	time = rescaleTime(time);
	int quarter = static_cast<int>(time) / 6;
	time -= quarter * 6;
	float factor1 = time / 6.0f;
	float factor2 = 1.0f - factor1;
	if (light1) {
		switch (quarter) {
			case 0:
				return factor1 * dawnColor;
			case 1:
				return factor2 * dawnColor + factor1 * dayColor;
			case 2:
				return factor2 * (factor2*dayColor + factor1 * dawnColor);
			case 3:
				return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}
	} else {
		switch (quarter) {
			case 0:
				return factor2 * (factor2 * nightColor + factor1 * duskColor);
			case 1:
				return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			case 2:
				return factor1 * duskColor;
			case 3:
				return factor2 * duskColor + factor1 * nightColor;
		}
	}
	throw std::exception("The precalculated quarter is out of the expected range! This should NEVER happen.");
}

void SunNode::updateWorld() {
	if (dirty) {
		glm::vec4 light1Color = timeToColor(time, true), light2Color = timeToColor(time, false);
		light1->ambient = light1Color * 0.1f;
		light1->diffuse = light1Color;
		light1->specular = light1Color * 0.5f;
		light2->ambient = light2Color * 0.1f;
		light2->diffuse = light2Color;
		light2->specular = light2Color * 0.5f;
		dynamic_cast<MeshSimple*>(light1Node->getMesh())->setColor(light1Color);
		dynamic_cast<MeshSimple*>(light2Node->getMesh())->setColor(light2Color);
		//localTransform.setMatrix(rotate(rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(rescaleTime(time) * 360.0f / 24.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
		localTransform.setMatrix(rotate(glm::mat4(1.0f), glm::radians(rescaleTime(time - 6.0f) * 360.0f / 24.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
		GraphNode::updateWorld();
	}
}
