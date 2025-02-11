//
// Created by MB on 4/28/2019.
//

#include "SunController.h"
#include "Ui/UiPlane.h"
#include "Serialization/Serializer.h"
#include "Scene/Components/LightComponents/Sun.h"
#include "Scene/SoundSystem.h"
#include "Ui/UiText.h"

SunController::SunController(GraphNode* _gameObject, Scene* scene, const std::string& name)
	:Component(_gameObject, name), scene(scene) {
	initialize();
}

SunController::SunController() {

}

void SunController::initialize() {
	sun = gameObject->getComponent<Sun>();
	if (sun != nullptr)
		sun->setTime(currentHour);
	clockBack = new UiPlane("res/textures/watch_sprite.png", glm::vec2(50.0f, 583.0f), glm::vec2(250.0f, 250.0f), Left);
	hint = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(clockBack->getPosition().x, clockBack->getPosition().y - 40.0f), glm::vec2(300.0f, 30.0f), TopLeft);
	hint->addChild(new UiText(hint->getSize() / 2.0f - glm::vec2(20.0f, 0.0f), hint->getSize(), "Use the watch with mouse scroll", glm::vec3(1.0f,1.0f,1.0f), MatchHeight));
	clockHand = new UiPlane("res/textures/watch_arrow_sprite.png", clockBack->getPosition() + clockBack->getSize() / 2.0f, glm::vec2(60.0f, 130.0f), Center);
	clockHand->setRotationAnchor(Center);
	clockHand->setActive(clockShown);
	clockBack->setActive(clockShown);
	scene->getUiRoot()->addChild(clockBack);
	scene->getUiRoot()->addChild(clockHand);

	clockHand->localTransform.setRotationZDegrees(currentClockHour);
}

void SunController::initializeTimeTravel(bool future) {
	timeTravel = true;
	startHour = sun->getTime();
	if (!future) {
		auto sound = SoundSystem::getSound("clockSound");
		sound->setVolume(0.15f);
		sound->play();
		endHour = startHour - 1.0f;
		clockStart = (startHour - 12.0f) * 30.0f;
		clockEnd = clockStart - 30.0f;
	} else {
		auto sound = SoundSystem::getSound("clockSound");
		sound->setVolume(0.15f);
		sound->play();
		endHour = startHour + 1.0f;
		clockStart = (startHour - 12.0f) * 30.0f;
		clockEnd = clockStart + 30.0f;
	}

}

SunController::~SunController() {
	/*delete clockFace;
	delete clockBack;
	delete clockHand;
	delete clockBackground;*/
}

void SunController::update(float msec) {
	GameManager *gameManager = GameManager::getInstance();

	if(clockShown && showHint) {
		if(hintLeft && hintRight) {
			setShowHint(false);
			gameManager->getCurrentNonEditorScene()->getUiRoot()->removeChild(hint);
		}else {
			gameManager->getCurrentNonEditorScene()->getUiRoot()->addChild(hint);
		}
	} else {
		gameManager->getCurrentNonEditorScene()->getUiRoot()->removeChild(hint);
	}

	if (gameManager->getScrollState() < -0.1f && !timeTravel) {
		if (sun != nullptr) {
			if (sun->getTime() != minHour && getFractionalPartOfHour() == 0.0f) {
				//setClockVisibility(true);
				//to the past
				hintLeft = true;
				initializeTimeTravel(false);
			}
		}
	}

	if (gameManager->getScrollState() > 0.1f && !timeTravel) {
		if (sun != nullptr) {
			if (sun->getTime() != maxHour && getFractionalPartOfHour() == 0.0f) {
				//setClockVisibility(true);
				//to the future
				hintRight = true;
				initializeTimeTravel(true);
			}
		}
	}

	if (timeTravel) {
		//clockHand->localTransform.rotateZ(0.8f * msec);
		//moveSun(1.6f * msec);
		if (GameManager::getInstance()->getKeyState(GLFW_KEY_LEFT_SHIFT))
			msec *= 1.45f;
		currentTimePercentage += msec;
		interpolate();
	}
}

void SunController::interpolate() {
	if (currentTimePercentage > 1.0f)
		currentTimePercentage = 1.0f;

	currentHour = glm::mix(startHour, endHour, currentTimePercentage);
	currentClockHour = glm::mix(clockStart, clockEnd, currentTimePercentage);

	clockHand->localTransform.setRotationZDegrees(currentClockHour);
	moveSun(currentHour);

	if (sun->getTime() == endHour) {
		timeTravel = false;
		currentTimePercentage = 0.0f;
	}
}

float SunController::getFractionalPartOfHour() {
	float integralPartOfHour;
	return modf(sun->getTime(), &integralPartOfHour);
}

void SunController::setClockVisibility(bool active) {
	//show clock
	clockShown = active;
	clockHand->setActive(clockShown);
	clockBack->setActive(clockShown);
}

void SunController::setShowHint(bool showHint) {
	this->showHint = showHint;
	if (showHint) {
		hintLeft = hintRight = false;
	}
}

bool SunController::getShowHint() {
	return showHint;
}

void SunController::renderGui() {
	Component::renderGui();
	if (isComponentActive()) {
		bool clockActive = clockShown;
		ImGui::Checkbox("Clock active", &clockActive);
		if (clockActive != clockShown) {
			setClockVisibility(clockActive);
		}
		bool showHint = this->showHint;
		ImGui::Checkbox("Show hint", &showHint);
		if(showHint != this->showHint) {
			setShowHint(showHint);
		}
	}
}

void SunController::moveSun(float time) {
	sun->setTime(time);
	if (sun->getTime() > maxHour)
		sun->setTime(maxHour);
	if (sun->getTime() < minHour)
		sun->setTime(minHour);
}


//SERIALIZATION

SerializableType SunController::getSerializableType() {
	return SSunController;
}

Json::Value SunController::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["scene"] = serializer->serialize(scene);
	root["startHour"] = startHour;
	root["endHour"] = endHour;
	root["clockStart"] = clockStart;
	root["clockEnd"] = clockEnd;
	root["currentTimePercentage"] = currentTimePercentage;
	root["currentHour"] = currentHour;
	root["currentClockHour"] = currentClockHour;
	root["timeTravel"] = timeTravel;
	root["clockShown"] = clockShown;
	root["showHint"] = showHint;
	root["hintLeft"] = hintLeft;
	root["hintRight"] = hintRight;
	return root;
}

void SunController::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	scene = dynamic_cast<Scene*>(serializer->deserialize(root["scene"]).object);
	startHour = root["startHour"].asFloat();
	endHour = root["endHour"].asFloat();
	clockStart = root["clockStart"].asFloat();
	clockEnd = root["clockEnd"].asFloat();
	currentTimePercentage = root["currentTimePercentage"].asFloat();
	currentHour = root["currentHour"].asFloat();
	currentClockHour = root["currentClockHour"].asFloat();
	timeTravel = root["timeTravel"].asBool();
	clockShown = root["clockShown"].asBool();
	showHint = root.get("showHint", false).asBool();
	hintLeft = root.get("hintLeft", false).asBool();
	hintRight = root.get("hintRight", false).asBool();
	initialize();
}


