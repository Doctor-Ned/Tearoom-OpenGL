#include "Scene/Scripts/Picking.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "Scene/CollisionSystem.h"
#include "Scene/GameManager.h"
#include "Scene/GraphNode.h"
#include "CollectableObject.h"
#include "Serialization/Serializer.h"
#include "Ui/UiPlane.h"
#include "Scene/Components/Animation.h"
#include "Scene/Components/AnimationController.h"
#include "Scene/SoundSystem.h"
#include "Scene/Components/Camera.h"
#include "CollectableWatch.h"


Picking::Picking(GraphNode* _gameObject, Camera* camera, Scene* scene, const std::string& name)
	: Component(_gameObject, name), scene(scene) {

	if (camera == nullptr) {
		camera = _gameObject->getComponent<Camera>();
	}

	this->camera = camera;

	UiElement *root = scene->getUiRoot();
	inventoryCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
	inventoryCanvas->setParent(root);
	inventoryCanvas->setActive(false);
	previewCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
	previewCanvas->setParent(root);
	previewCanvas->setActive(false);
	photosInventory = new UiPlane("res/textures/photosChosenInventory.PNG", glm::vec2(1285.0f, 580.0f), glm::vec2(390.0f, 300.0f), Right);
	itemsInventory = new UiPlane("res/textures/itemsChosenInventory.PNG", glm::vec2(1285.0f, 580.0f), glm::vec2(390.0f, 300.0f), Right);
	letterInventory = new UiPlane("res/textures/lettersChosenInventory.PNG", glm::vec2(1285.0f, 580.0f), glm::vec2(390.0f, 300.0f), Right);
	itemsButton = new UiButton(glm::vec2(1006.0f, 475.0f), glm::vec2(80.0f, 40.0f), Right);
	letterButton = new UiButton(glm::vec2(1126.0f, 475.0f), glm::vec2(100.0f, 40.0f), Right);
	photoButton = new UiButton(glm::vec2(1246.0f, 475.0f), glm::vec2(90.0f, 40.0f), Right);
	descBackground = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(1295.0f, 355.0f), glm::vec2(400.0f, 150.0f), Right);

	itemsButton->addClickCallback([this]() {
		letterInventory->setActive(false);
		itemsInventory->setActive(true);
		photosInventory->setActive(false);
		showInventoryUi();
		SoundSystem::getSound("bow")->setDefaultVolume(0.03f);
		SoundSystem::getEngine()->play2D(SoundSystem::getSound("bow"));
	});

	letterButton->addClickCallback([this]() {
		itemsInventory->setActive(false);
		photosInventory->setActive(false);
		letterInventory->setActive(true);
		showInventoryUi();
		SoundSystem::getSound("bow")->setDefaultVolume(0.03f);
		SoundSystem::getEngine()->play2D(SoundSystem::getSound("bow"));
	});
	photoButton->addClickCallback([this]() {
		letterInventory->setActive(false);
		itemsInventory->setActive(false);
		photosInventory->setActive(true);
		showInventoryUi();
		SoundSystem::getSound("bow")->setDefaultVolume(0.03f);
		SoundSystem::getEngine()->play2D(SoundSystem::getSound("bow"));
	});

	inventoryCanvas->addChild(itemsButton);
	inventoryCanvas->addChild(letterButton);
	inventoryCanvas->addChild(photoButton);
	inventoryCanvas->addChild(photosInventory);
	inventoryCanvas->addChild(itemsInventory);
	inventoryCanvas->addChild(letterInventory);

	encouragementCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
	encouragementCanvas->setParent(root);
	encouragementBackground = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(720.0f, 260.0f), glm::vec2(200.0f, 30.0f), Center);
	encouragementPick = new UiText(glm::vec2(700.0f, 260.0f), glm::vec2(60.0f, 30.0f), "Press E to pick up", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);
	encouragementActivate = new UiText(glm::vec2(700.0f, 260.0f), glm::vec2(60.0f, 30.0f), "Press E to interact", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);

	descBackground->setActive(false);
	encouragementCanvas->setActive(false);
	encouragementCanvas->addChild(encouragementBackground);
	encouragementCanvas->addChild(encouragementPick);
	encouragementCanvas->addChild(encouragementActivate);

    inventoryCanvas->addChild(descBackground);

	GameManager::getInstance()->addKeyCallback(GLFW_KEY_I, true, [this]() {
		this->scene->setCursorLocked(!(this->scene->getCursorLocked()));
		encouragementActivate->setText("Press E to interact");
		setSwitch(!getSwitch());
	});
	GameManager::getInstance()->addKeyCallback(GLFW_KEY_I, false, [this]() {
		if (getSwitch()) {
			previewCanvas->setActive(false);
			showInventoryUi();
		} else {
			hideInventoryUi();
		}
	});

    // for demo purposes
    UiPlane* firstPhotoIcon = new UiPlane("res/textures/Icons/letterIcon.png", glm::vec2(995.0f, 530.0f), glm::vec2(60.0f, 60.0f), Right);
    UiPlane* firstPhotoPreview = new UiPlane("res/textures/Photos/13thHour.jpg", glm::vec2(1200.0f, 430.0f), glm::vec2(300.0f, 500.0f), Right);
    UiPlane* firstLetterPreview = new UiPlane("res/textures/Letter/firstLetterPreview.PNG", glm::vec2(1200.0f, 430.0f), glm::vec2(300.0f, 450.0f), Right);

    GraphNode* firstPhoto = new GraphNode(nullptr, scene->getRootNode());
    GraphNode* firstLetter = new GraphNode(nullptr, scene->getRootNode());

    CollectableObject* colPhoto = new CollectableObject(firstPhoto, Photo, firstPhotoIcon, "Photo from uncle Yoshiro", firstPhotoPreview);
    CollectableObject* colLetter = new CollectableObject(firstLetter, Letter, firstPhotoIcon, "Letter from uncle Yoshiro", firstLetterPreview);
    firstPhoto->addComponent(colPhoto);
    firstLetter->addComponent(colLetter);
    firstPhoto->setActive(false);
    firstLetter->setActive(false);
    colPhoto->setButton(new UiButton(glm::vec2(1006.0f, 475.0f), glm::vec2(60.0f, 60.0f), Right));
    colPhoto->getButton()->setOpacity(0.0f);
    colLetter->setButton(new UiButton(glm::vec2(1006.0f, 475.0f), glm::vec2(60.0f, 60.0f), Right));
    colLetter->getButton()->setOpacity(0.0f);
    inventory.push_back(firstPhoto);
    inventory.push_back(firstLetter);

    colPhoto->getButton()->addClickCallback([this, colPhoto]() {
        this->scene->setCursorLocked(!(this->scene->getCursorLocked()));
        setSwitch(!getSwitch());
        hideInventoryUi();
        previewCanvas->addChild(colPhoto->getPreview());
        previewCanvas->setActive(true);
    });

    colLetter->getButton()->addClickCallback([this, colLetter]() {
        this->scene->setCursorLocked(!(this->scene->getCursorLocked()));
        setSwitch(!getSwitch());
        hideInventoryUi();
        previewCanvas->addChild(colLetter->getPreview());
        previewCanvas->setActive(true);
    });

    previewCanvas->addChild(firstPhotoPreview);
    previewCanvas->setActive(true);

}

void Picking::placeInGrid(ItemType itype) {
	int i = 0;
	for (GraphNode *obj : inventory) {
		CollectableObject *col = obj->getComponent<CollectableObject>();
		if (col->getI_type() == itype || (col->getI_type() == DoorKey && itype == NormalItem)) {

			if (i >= 8) {
				col->getIcon()->setPosition(glm::vec2(995.0f + (81.0f * (i - 8)), 664.0f));
				col->getButton()->setPosition(glm::vec2(995.0f + (81.0f * (i - 8)), 664.0f));
			} else if (i >= 4) {
				col->getIcon()->setPosition(glm::vec2(995.0f + (81.0f * (i - 4)), 597.0f));
				col->getButton()->setPosition(glm::vec2(995.0f + (81.0f * (i - 4)), 597.0f));
			} else {
				col->getIcon()->setPosition(glm::vec2(995.0f + (81.0f * i), 530.0f));
				col->getButton()->setPosition(glm::vec2(995.0f + (81.0f * i), 530.0f));
			}
			inventoryCanvas->addChild(col->getButton());
			inventoryCanvas->addChild(col->getIcon());
			i++;
		} else {
			inventoryCanvas->removeChild(col->getIcon());
			inventoryCanvas->removeChild(col->getButton());
		}
	}
}

void Picking::collect(CollectableObject* collectable) {
	inventory.push_back(collectable->getGameObject());
	collectable->setButton(new UiButton(glm::vec2(1006.0f, 475.0f), glm::vec2(60.0f, 60.0f), Right));
	collectable->getButton()->setOpacity(0.0f);

	if (collectable->getI_type() == Letter || collectable->getI_type() == Photo) {
		collectable->getButton()->addClickCallback([this, collectable]() {
			this->scene->setCursorLocked(!(this->scene->getCursorLocked()));
			setSwitch(!getSwitch());
			hideInventoryUi();
			previewCanvas->addChild(collectable->getPreview());
			previewCanvas->setActive(true);
		});

	} else if (collectable->getI_type() == DoorKey) {
		collectable->getButton()->addClickCallback([this, collectable]() {
			if (currentInteraction != nullptr) {
				AnimationController *anim = currentInteraction->getComponent<AnimationController>();
				if (collectable->getDoorID() == anim->getDoorID()) {
					anim->playAnimation();
					hideInventoryUi();
					for (int i = 0; i < inventory.size(); i++) {
						auto obj = inventory[i]->getComponent<CollectableObject>();
						if (obj->getDoorID() == anim->getDoorID()) {
							inventory.erase(inventory.begin() + i);
							this->scene->setCursorLocked(!(this->scene->getCursorLocked()));
						}
					}
				}
			}
		});
	} else {
		this->scene->setCursorLocked(!(this->scene->getCursorLocked()));
		setSwitch(!getSwitch());
		hideInventoryUi();
	}
	collectable->getButton()->addHoverCallback([this, collectable]() {
		descBackground->setActive(true);
		descBackground->addChild(collectable->getDescription());

	});
	collectable->getButton()->addLeaveCallback([this, collectable]() {
		descBackground->setActive(false);
		descBackground->removeChild(collectable->getDescription());
	});
	collectable->takeObject();

	if (inventoryUI) {
		if (itemsInventory->isActive() && collectable->getI_type() == NormalItem) {
			inventoryCanvas->addChild(collectable->getButton());
			inventoryCanvas->addChild(collectable->getIcon());
			showInventoryUi();
		}
		if (letterInventory->isActive() && collectable->getI_type() == Letter) {
			inventoryCanvas->addChild(collectable->getButton());
			inventoryCanvas->addChild(collectable->getIcon());
			showInventoryUi();
		}
		if (photosInventory->isActive() && collectable->getI_type() == Photo) {
			inventoryCanvas->addChild(collectable->getButton());
			inventoryCanvas->addChild(collectable->getIcon());
			showInventoryUi();
		}
	}
}

Picking::Picking(GraphNode* _gameObject, const std::string& name) : Picking(_gameObject, gameManager->getCurrentNonEditorCamera(), gameManager->getCurrentNonEditorScene(), name) {}


void Picking::hideInventoryUi() {
	inventoryCanvas->setActive(false);
	for (GraphNode* obj : inventory) {
		inventoryCanvas->removeChild(obj->getComponent<CollectableObject>()->getIcon());
	}
}

void Picking::showInventoryUi() {
	inventoryCanvas->setActive(true);
	previewCanvas->setActive(false);

	if (letterInventory->isActive()) {
		placeInGrid(Letter);
	}
	if (itemsInventory->isActive()) {
		placeInGrid(NormalItem);
	}
	if (photosInventory->isActive()) {
		placeInGrid(Photo);
	}
}

bool Picking::getSwitch() {
	return inventoryUI;
}

void Picking::hidePreview() {
    previewCanvas->setActive(false);
}

void Picking::showPreview() {
    previewCanvas->setActive(true);
}

void Picking::setSwitch(bool ifShown) {
	this->inventoryUI = ifShown;
}


void Picking::update(float msec) {
	GameManager *gameManager = GameManager::getInstance();

	Collider* coll = gameObject->getComponent<Collider>();
	GraphNode * object = CollisionSystem::getInstance()->castRay(camera->getGameObject()->worldTransform.getPosition(), camera->getGameObject()->getFrontVector(), 2.0f, coll);

	encouragementCanvas->setActive(false);
	if (object && object->isActive()) {
		CollectableObject *collectable = object->getComponent<CollectableObject>();
		if (collectable && collectable->isComponentActive()) {
			encouragementCanvas->setActive(true);
			encouragementActivate->setActive(false);
            encouragementPick->setActive(true);

			if (gameManager->getKeyOnce(GLFW_KEY_E) && !collectable->getIsTaken()) {
				collect(collectable);
			}
		}

		CollectableWatch *watch = object->getComponent<CollectableWatch>();
		if(watch && watch->isComponentActive()) {
			watch->pickup();
			encouragementCanvas->setActive(true);
			encouragementActivate->setActive(true);
			encouragementPick->setActive(false);
			encouragementActivate->setText("Press E to take the watch");
			if (gameManager->getKeyOnce(GLFW_KEY_E)) {
				watch->pickup();
				collect(watch->getCollectable());
				previewCanvas->addChild(collectable->getPreview());
				previewCanvas->setActive(true);
			}
			//todo: show the letter
		}

		Animation* anim = object->getComponent<Animation>();
        AnimationController* animController = object->getComponent<AnimationController>();
		
		if (anim && anim->isComponentActive() && !anim->getIsPlaying() && !animController) {
			encouragementCanvas->setActive(true);
			encouragementActivate->setActive(true);
			encouragementPick->setActive(false);
			encouragementActivate->setText("Press E to interact");

			if (gameManager->getKeyOnce(GLFW_KEY_E)) {
				anim->play();
			}
		}
		 

		if (animController && animController->isComponentActive()) {
			encouragementCanvas->setActive(true);
			encouragementActivate->setActive(true);
			encouragementPick->setActive(false);

			if (gameManager->getKeyOnce(GLFW_KEY_E)) {
				if (animController->getType() == DoorOpeningX || animController->getType() == DoorOpeningY) {
                    encouragementActivate->setText("Oops.. I need a key");
                    this->scene->setCursorLocked(!(this->scene->getCursorLocked()));
					setSwitch(!getSwitch());
					showInventoryUi();
					currentInteraction = object;
				}
			}
		}
		else {

		}
	}
}

Picking::~Picking() {}

SerializableType Picking::getSerializableType() {
	return SPicking;
}

Json::Value Picking::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["camera"] = serializer->serialize(camera);
	root["scene"] = serializer->serialize(scene);
	return root;
}

void Picking::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	camera = dynamic_cast<Camera*>(serializer->deserialize(root["camera"]).object);
	scene = dynamic_cast<Scene*>(serializer->deserialize(root["scene"]).object);
	UiElement *uiRoot = scene->getUiRoot();
	inventoryCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), uiRoot->getSize());
	inventoryCanvas->setParent(uiRoot);
	inventoryCanvas->setActive(false);
	previewCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), uiRoot->getSize());
	previewCanvas->setParent(uiRoot);
	previewCanvas->setActive(false);
	photosInventory = new UiPlane("res/textures/photosChosenInventory.PNG", glm::vec2(1285.0f, 580.0f), glm::vec2(390.0f, 300.0f), Right);
	itemsInventory = new UiPlane("res/textures/itemsChosenInventory.PNG", glm::vec2(1285.0f, 580.0f), glm::vec2(390.0f, 300.0f), Right);
	letterInventory = new UiPlane("res/textures/lettersChosenInventory.PNG", glm::vec2(1285.0f, 580.0f), glm::vec2(390.0f, 300.0f), Right);
	itemsButton = new UiButton(glm::vec2(1006.0f, 475.0f), glm::vec2(80.0f, 40.0f), Right);
	letterButton = new UiButton(glm::vec2(1126.0f, 475.0f), glm::vec2(100.0f, 40.0f), Right);
	photoButton = new UiButton(glm::vec2(1246.0f, 475.0f), glm::vec2(90.0f, 40.0f), Right);
	descBackground = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(1295.0f, 355.0f), glm::vec2(400.0f, 150.0f), Right);

	itemsButton->addClickCallback([this]() {
		letterInventory->setActive(false);
		itemsInventory->setActive(true);
		photosInventory->setActive(false);
		showInventoryUi();
		SoundSystem::getSound("bow")->setDefaultVolume(0.15);
		SoundSystem::getEngine()->play2D(SoundSystem::getSound("bow"));
	});
	letterButton->addClickCallback([this]() {
		itemsInventory->setActive(false);
		photosInventory->setActive(false);
		letterInventory->setActive(true);
		showInventoryUi();
		SoundSystem::getSound("bow")->setDefaultVolume(0.15f);
		SoundSystem::getEngine()->play2D(SoundSystem::getSound("bow"));
	});
	photoButton->addClickCallback([this]() {
		letterInventory->setActive(false);
		itemsInventory->setActive(false);
		photosInventory->setActive(true);
		showInventoryUi();
		SoundSystem::getSound("bow")->setDefaultVolume(0.15);
		SoundSystem::getEngine()->play2D(SoundSystem::getSound("bow"));
	});

	inventoryCanvas->addChild(itemsButton);
	inventoryCanvas->addChild(letterButton);
	inventoryCanvas->addChild(photoButton);
	inventoryCanvas->addChild(photosInventory);
	inventoryCanvas->addChild(itemsInventory);
	inventoryCanvas->addChild(letterInventory);

	encouragementCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), uiRoot->getSize());
	encouragementCanvas->setParent(uiRoot);
	encouragementBackground = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(720.0f, 260.0f), glm::vec2(200.0f, 30.0f), Center);
	encouragementPick = new UiText(glm::vec2(700.0f, 260.0f), glm::vec2(60.0f, 30.0f), "Press E to pick up", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);
	encouragementActivate = new UiText(glm::vec2(700.0f, 260.0f), glm::vec2(60.0f, 30.0f), "Press E to interact", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);

	descBackground->setActive(false);
	encouragementCanvas->setActive(false);
	encouragementCanvas->addChild(encouragementBackground);
	encouragementCanvas->addChild(encouragementPick);
	encouragementCanvas->addChild(encouragementActivate);

    inventoryCanvas->addChild(descBackground);

	GameManager::getInstance()->addKeyCallback(GLFW_KEY_I, true, [this]() {
		GameManager::getInstance()->setCursorLocked(!(GameManager::getInstance()->getCursorLocked()));
        encouragementActivate->setText("Press E to interact");
        setSwitch(!getSwitch());
	});
	GameManager::getInstance()->addKeyCallback(GLFW_KEY_I, false, [this]() {
		if (getSwitch()) {
			previewCanvas->setActive(false);
			showInventoryUi();
		} else {
			hideInventoryUi();
		}
	});
}
Scene *Picking::getScene() const {
	return scene;
}
UiCanvas *Picking::getInventoryCanvas() const {
	return inventoryCanvas;
}

void Picking::setCamera(Camera* camera) {
	this->camera = camera;
}

Camera* Picking::getCamera() {
	return camera;
}
