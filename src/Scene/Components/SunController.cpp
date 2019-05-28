//
// Created by MB on 4/28/2019.
//

#include "SunController.h"
#include "Ui/UiPlane.h"
#include "Serialization/Serializer.h"
#include "Scene/Components/LightComponents/Sun.h"

SunController::SunController(GraphNode* _gameObject, Scene* scene, const std::string& name)
:Component(_gameObject, name), scene(scene){
	initialize();
}

SunController::SunController()
{

}

void SunController::initialize()
{
	sun = gameObject->getComponent<Sun>();
	if (sun != nullptr)
		sun->setTime(12.0f);
	clockFace = new UiPlane("res/textures/clockFace.png", glm::vec2(50.0f, 580.0f), glm::vec2(200.0f, 200.0f), Left);
	clockHand = new UiPlane("res/textures/clockHand.png", clockFace->getPosition() + clockFace->getSize() / 2.0f, glm::vec2(60.0f, 130.0f), Center);
	clockBack = new UiPlane("res/textures/clockBack.png", glm::vec2(-24.0f, 583.0f), glm::vec2(345.0f, 350.0f), Left);
	clockHand->setRotationAnchor(Center);
	clockFace->setActive(false);
	clockHand->setActive(false);
	clockBack->setActive(false);

	scene->getUiRoot()->addChild(clockBack);
	scene->getUiRoot()->addChild(clockFace);
	scene->getUiRoot()->addChild(clockHand);

	GameManager::getInstance()->addKeyCallback(GLFW_KEY_1, true, [this]()
	{
		clockFace->setActive(true);
		clockHand->setActive(true);
		clockBack->setActive(true);
	});
	GameManager::getInstance()->addKeyCallback(GLFW_KEY_2, true, [this]()
	{
		clockFace->setActive(true);
		clockHand->setActive(true);
		clockBack->setActive(true);
	});
	GameManager::getInstance()->addKeyCallback(GLFW_KEY_1, false, [this]()
	{
		clockFace->setActive(false);
		clockHand->setActive(false);
		clockBack->setActive(false);
	});
	GameManager::getInstance()->addKeyCallback(GLFW_KEY_2, false, [this]()
	{
		clockFace->setActive(false);
		clockHand->setActive(false);
		clockBack->setActive(false);
	});
}

SunController::~SunController() {
	/*delete clockFace;
	delete clockBack;
	delete clockHand;
	delete clockBackground;*/
}

//methods for sunlight control
void SunController::moveSun(float time)
{
	if (sun != nullptr)
	{
		sun->addTime(time);
		if (sun->getTime() > 17)
			sun->setTime(17.0f);
		if (sun->getTime() < 7)
			sun->setTime(7.0f);
	}
}

void SunController::update(float msec) {
    GameManager *gameManager = GameManager::getInstance();


    if(gameManager->getKeyState(GLFW_KEY_1))
    {
		if (sun != nullptr)
		{
			if(sun->getTime() != 7.0f)
			{
				clockHand->localTransform.rotateZ(-0.8f * msec);
				moveSun(-1.6f * msec);
			}
		}
        
    }

    if(gameManager->getKeyState(GLFW_KEY_2))
    {
		if (sun != nullptr)
		{
			if (sun->getTime() != 17.0f)
			{
				clockHand->localTransform.rotateZ(0.8f * msec);
				moveSun(1.6f * msec);
			}
		}
    }
}

//SERIALIZATION

SerializableType SunController::getSerializableType() {
	return SSunController;
}

Json::Value SunController::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["scene"] = serializer->serialize(scene);
	//sth to serialize in this class
	return root;
}

void SunController::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	scene = dynamic_cast<Scene*>(serializer->deserialize(root["scene"]).object);
	initialize();
}


