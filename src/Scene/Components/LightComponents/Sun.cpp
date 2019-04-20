#include "Sun.h"
#include "Render/LightManager.h"
#include "Mesh/MeshColorSphere.h"
#include "Scene/GraphNode.h"

Sun::Sun(DirLight* light1, DirLight* light2, glm::vec4 dawnColor, glm::vec4 dayColor, glm::vec4 duskColor,
	glm::vec4 nightColor, float sunDistance, float initialTime, GraphNode *gameObject, DirLightComp *dirLightComp1, DirLightComp *dirLightComp2) : Component(gameObject), light1(light1), light2(light2),
	dawnColor(dawnColor), dayColor(dayColor), duskColor(duskColor), nightColor(nightColor), sunDistance(sunDistance), light1Comp(dirLightComp1), light2Comp(dirLightComp2) {
	setTime(initialTime);
}

Sun::Sun(DirLight* light1, DirLight* light2, glm::vec4 dawnColor, glm::vec4 dayColor, glm::vec4 duskColor,
	glm::vec4 nightColor, float sunDistance, float initialTime, GraphNode *gameObject) : Component(gameObject), light1(light1), light2(light2),
	dawnColor(dawnColor), dayColor(dayColor), duskColor(duskColor), nightColor(nightColor), sunDistance(sunDistance) {
	setTime(initialTime);
	GraphNode *light1Node = new GraphNode(new MeshColorSphere(0.25f, 30, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)), gameObject);
	light1Node->getMesh()->setShaderType(STLight);
	GraphNode *light2Node = new GraphNode(new MeshColorSphere(0.25f, 30, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)), gameObject);
	light2Node->getMesh()->setShaderType(STLight);
	light1Comp = new DirLightComp(light1, light1Node);
	light1Node->addComponent(light1Comp);
	light2Comp = new DirLightComp(light2, light2Node);
	light2Node->addComponent(light2Comp);
	light1Comp->getGameObject()->localTransform.setMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -sunDistance)));
	light1Comp->getGameObject()->localTransform.rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	light2Comp->getGameObject()->localTransform.setMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, sunDistance)));
}

void Sun::setTime(float time) {
	this->time = time;
	dirty = true;
}

void Sun::addTime(float time) {
	this->time = this->time + time;
	dirty = true;
}

void Sun::setVerticalRotation(float angle) {
	this->rotationAngle = angle;
	dirty = true;
}

float Sun::getTime() {
	return time;
}

void Sun::update(float msec) {
	if (dirty) {
		glm::vec4 light1Color = timeToColor(time, true), light2Color = timeToColor(time, false);
		light1->ambient = light1Color * 0.1f;
		light1->diffuse = light1Color;
		light1->specular = light1Color * 0.5f;
		light2->ambient = light2Color * 0.1f;
		light2->diffuse = light2Color;
		light2->specular = light2Color * 0.5f;
		dynamic_cast<MeshSimple*>(light1Comp->getGameObject()->getMesh())->setColor(light1Color);
		dynamic_cast<MeshSimple*>(light2Comp->getGameObject()->getMesh())->setColor(light2Color);
		glm::vec4 position = gameObject->localTransform.getMatrix()[3];
		gameObject->localTransform.setMatrix(rotate(translate(glm::mat4(1.0f), glm::vec3(position)), glm::radians(rescaleTime(time - 6.0f) * 360.0f / 24.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	}
}

float Sun::rescaleTime(float time) {
	while (time >= 24.0f) {
		time -= 24.0f;
	}
	while (time < 0.0f) {
		time += 24.0f;
	}
	return time;
}

glm::vec4 Sun::timeToColor(float time, bool light1) {
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
