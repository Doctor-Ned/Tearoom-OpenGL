#include "CollectableWatch.h"
#include "Scene/Components/SunController.h"

CollectableWatch::CollectableWatch(GraphNode* _gameObject) : Component(_gameObject, "Collectable Watch") { }
void CollectableWatch::update(float msec) {}

void CollectableWatch::pickup() {
	if (sunController) {
		sunController->setClockVisibility(true);
		sunController->setComponentActive(true);
	}
	this->setComponentActive(false);
}

SerializableType CollectableWatch::getSerializableType() {
	return SNone;
}

Json::Value CollectableWatch::serialize(Serializer* serializer) {
	//todo
	return Json::Value();
}

void CollectableWatch::deserialize(Json::Value& root, Serializer* serializer) {
	//todo
}

void CollectableWatch::setSunController(SunController* sunController) {
	this->sunController = sunController;
}

SunController* CollectableWatch::getSunController() {
	return sunController;
}

void CollectableWatch::renderGui() {
	Component::renderGui();
	if(isComponentActive()) {
		
	}
}
