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
#include "Profiler.h"
#include "AnimTimeSaver.h"


Picking::Picking(GraphNode* _gameObject, Camera* camera, Scene* scene, const std::string& name)
	: Component(_gameObject, name), scene(scene) {

	if (camera == nullptr) {
		camera = _gameObject->getComponent<Camera>();
	}

	this->camera = camera;
	initialize();
}

void Picking::placeInGrid(ItemType itype, UiCanvas* canvas) {
	int i = 0;
	for (GraphNode *obj : inventory) {
		CollectableObject *col = obj->getComponent<CollectableObject>();
		if (col->getI_type() == itype || (col->getI_type() == NormalItem && itype == Letter)) {
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
			col->getButton()->setActive(true);
			col->getIcon()->setActive(true);
			if(col->getI_type() == NormalItem && col->getPreview() != nullptr) {
				col->getButton()->addClickCallback([this, col]() {
					setButtonCallbackBody(col);
				});
			} else if (col->getI_type() == Letter || col->getI_type() == Photo) {
				col->getButton()->addClickCallback([this, col]() {
					setButtonCallbackBody(col);
				});
			} else if (col->getI_type() == DoorKey) {
				col->getButton()->addClickCallback([this, col]() {
					if (currentInteraction != nullptr) {
						AnimationController *anim = currentInteraction->getComponent<AnimationController>();
						if (anim->isComponentActive() && col->getDoorID() == anim->getDoorID()) {
							anim->open();
							inventoryUI = false;
							hideInventoryUi();
							gameManager->setCursorLocked(true);
							//this->scene->setCursorLocked(!(this->scene->getCursorLocked()));
							for (int i = 0; i < inventory.size(); i++) {
								auto obj = inventory[i]->getComponent<CollectableObject>();
								if (obj->getDoorID() == anim->getDoorID()) {
									inventory.erase(inventory.begin() + i);
								}
							}
						}
					}
				});
			}

			canvas->addChild(col->getButton());
			canvas->addChild(col->getIcon());
			i++;
		} else {
			col->getButton()->setActive(false);
			col->getIcon()->setActive(false);
			col->getButton()->clearCallbacks();
		}
	}
}

void Picking::initialize() {
	UiElement *root = scene->getUiRoot();
	
	root->addChild(new UiText(glm::vec2(UI_REF_CEN_X, UI_REF_CEN_Y), "+", 1.0f));
	//each canvas represents different inventory section
	letterInventoryCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
	photosInventoryCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
	itemsInventoryCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());

	buttonsCanvas = new  UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());

	buttonsCanvas->setParent(root);
	letterInventoryCanvas->setParent(root);
	photosInventoryCanvas->setParent(root);
	itemsInventoryCanvas->setParent(root);

	//canvas for photos/letters preview
	previewCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
	previewCanvas->setParent(root);
	previewCanvas->setActive(false);

	photosInventory = new UiPlane("res/textures/photosChosenInventory.PNG", glm::vec2(1285.0f, 580.0f), glm::vec2(390.0f, 300.0f), Right);
	itemsInventory = new UiPlane("res/textures/itemsChosenInventory.PNG", glm::vec2(1285.0f, 580.0f), glm::vec2(390.0f, 300.0f), Right);
	letterInventory = new UiPlane("res/textures/loreChosenInventory.PNG", glm::vec2(1285.0f, 580.0f), glm::vec2(390.0f, 300.0f), Right);

	photosInventoryCanvas->addChild(photosInventory);
	letterInventoryCanvas->addChild(letterInventory);
	itemsInventoryCanvas->addChild(itemsInventory);
	currentCanvas = itemsInventoryCanvas; //!!!

	//BUTTONS AND THEIR CALLBACKS
	itemsButton = new UiButton(glm::vec2(1006.0f, 475.0f), glm::vec2(80.0f, 40.0f), Right);
	letterButton = new UiButton(glm::vec2(1246.0f, 475.0f), glm::vec2(100.0f, 40.0f), Right);
	photoButton = new UiButton(glm::vec2(1126.0f, 475.0f), glm::vec2(90.0f, 40.0f), Right);
	descBackground = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(1295.0f, 355.0f), glm::vec2(400.0f, 150.0f), Right);

	letterButton->setOpacity(0.0f);
	itemsButton->setOpacity(0.0f);
	photoButton->setOpacity(0.0f);

	itemsButton->addClickCallback([this]() {
		auto sound = SoundSystem::getSound("clickSound");
		sound->setVolume(0.15f);
		sound->play();
		letterInventoryCanvas->setActive(false);
        photosInventoryCanvas->setActive(false);
		currentCanvas = itemsInventoryCanvas;
		showInventoryUi();
	});
	letterButton->addClickCallback([this]() {
		auto sound = SoundSystem::getSound("clickSound");
		sound->setVolume(0.15f);
		sound->play();
		itemsInventoryCanvas->setActive(false);
		photosInventoryCanvas->setActive(false);
		currentCanvas = letterInventoryCanvas;
		showInventoryUi();

	});
	photoButton->addClickCallback([this]() {
		auto sound = SoundSystem::getSound("clickSound");
		sound->setVolume(0.15f);
		sound->play();
		letterInventoryCanvas->setActive(false);
		itemsInventoryCanvas->setActive(false);

        currentCanvas = photosInventoryCanvas;
		showInventoryUi();
	});

	buttonsCanvas->addChild(itemsButton);
    buttonsCanvas->addChild(letterButton);
    buttonsCanvas->addChild(photoButton);
    buttonsCanvas->addChild(descBackground);

	encouragementCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
	encouragementCanvas->setParent(root);
	encouragementBackground = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(770.0f, 260.0f), glm::vec2(200.0f, 30.0f), Center);
	encouragementPick = new UiText(glm::vec2(750.0f, 260.0f), glm::vec2(60.0f, 30.0f), "Click to pick up", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);
	encouragementActivate = new UiText(glm::vec2(750.0f, 260.0f), glm::vec2(60.0f, 30.0f), "Click to interact", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);

	descBackground->setActive(false);
	encouragementCanvas->setActive(false);
	encouragementCanvas->addChild(encouragementBackground);
	encouragementCanvas->addChild(encouragementPick);
	encouragementCanvas->addChild(encouragementActivate);

	GameManager::getInstance()->addMouseCallback(GLFW_MOUSE_BUTTON_RIGHT, true, [this]() {
		encouragementActivate->setText("Click to interact");
		gameManager->setCursorLocked(inventoryUI);
		inventoryUI = !inventoryUI;
		if (getSwitch()) {
			previewCanvas->setActive(false);
            buttonsCanvas->setActive(true);
            showInventoryUi();
		} else {
			hideInventoryUi();
            buttonsCanvas->setActive(false);

        }
	});

	// for demo purposes
	GraphNode* firstPhoto = new GraphNode(nullptr, scene->getRootNode());
	GraphNode* firstLetter = new GraphNode(nullptr, scene->getRootNode());

	CollectableObject* colPhoto = new CollectableObject(firstPhoto, Photo, "res/textures/Photos/13thHour.jpg", glm::vec2(995.0f, 530.0f), glm::vec2(60.0f, 60.0f), "Photo from uncle Yoshiro", "res/textures/Photos/13thHour.jpg", glm::vec2(1200.0f, 430.0f), glm::vec2(300.0f, 500.0f));
    colPhoto->setPhotoID(1);
	CollectableObject* colLetter = new CollectableObject(firstLetter, Letter, "res/textures/Letter/firstLetterPreview.PNG", glm::vec2(995.0f, 530.0f), glm::vec2(60.0f, 60.0f), "Letter from uncle Yoshiro", "res/textures/Letter/firstLetterPreview.PNG", glm::vec2(1200.0f, 430.0f), glm::vec2(300.0f, 450.0f));
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
		setButtonCallbackBody(colPhoto);
	});

	colLetter->getButton()->addClickCallback([this, colLetter]() {
		setButtonCallbackBody(colLetter);
	});

	previewCanvas->addChild(colPhoto->getPreview());
	previewCanvas->setActive(true);
    buttonsCanvas->setActive(false);
    letterInventoryCanvas->setActive(false);
	photosInventoryCanvas->setActive(false);
	itemsInventoryCanvas->setActive(false);
}

void Picking::collect(CollectableObject* collectable) {
	inventory.push_back(collectable->getGameObject());
	collectable->setButton(new UiButton(glm::vec2(1006.0f, 475.0f), glm::vec2(60.0f, 60.0f), Right));
	collectable->getButton()->setOpacity(0.0f);

	collectable->getButton()->addHoverCallback([this, collectable]() {
		descBackground->setActive(true);
		descBackground->addChild(collectable->getDescription());
	});
	collectable->getButton()->addLeaveCallback([this, collectable]() {
		descBackground->setActive(false);
		descBackground->removeChild(collectable->getDescription());
	});

	collectable->takeObject();

	//adding item to grid while opened
	if (inventoryUI) {
		if (itemsInventory->isActive() && collectable->getI_type() == NormalItem) {
			itemsInventoryCanvas->addChild(collectable->getButton());
			itemsInventoryCanvas->addChild(collectable->getIcon());
			showInventoryUi();
		}
		if (letterInventory->isActive() && collectable->getI_type() == Letter) {
			letterInventoryCanvas->addChild(collectable->getButton());
			letterInventoryCanvas->addChild(collectable->getIcon());
			showInventoryUi();
		}
		if (photosInventory->isActive() && collectable->getI_type() == Photo) {
			photosInventoryCanvas->addChild(collectable->getButton());
			photosInventoryCanvas->addChild(collectable->getIcon());
			showInventoryUi();
		}
	}
}

Picking::Picking(GraphNode* _gameObject, const std::string& name) : Picking(_gameObject, gameManager->getCurrentNonEditorCamera(), gameManager->getCurrentNonEditorScene(), name) {}

void Picking::setButtonCallbackBody(CollectableObject *collectable) {
	gameManager->setCursorLocked(true);
	inventoryUI = false;
	hideInventoryUi();
	for (UiElement* elem : previewCanvas->getChildren()) {
		previewCanvas->removeChild(elem);
	}
	previewCanvas->addChild(collectable->getPreview());
	previewCanvas->setActive(true);
	auto sound = SoundSystem::getSound("previewSound");
	sound->setVolume(0.80f);
	sound->play();
}

void Picking::hideInventoryUi() {
	currentCanvas->setActive(false);
	//currentCanvas = itemsInventoryCanvas;
}

void Picking::showInventoryUi() {
	previewCanvas->setActive(false);
	currentCanvas->setActive(true);

	if (letterInventoryCanvas->isActive()) {
		placeInGrid(Letter, letterInventoryCanvas);
	}
	if (itemsInventoryCanvas->isActive()) {
		placeInGrid(DoorKey, itemsInventoryCanvas);
	}
	if (photosInventoryCanvas->isActive()) {
		placeInGrid(Photo, photosInventoryCanvas);
	}
}

bool Picking::getSwitch() {
	return inventoryUI;
}

void Picking::hidePreview() {
	previewCanvas->setActive(false);
}

void Picking::showPreview() {
	auto sound = SoundSystem::getSound("previewSound");
	sound->setVolume(0.15f);
	sound->play();
	previewCanvas->setActive(true);
}

void Picking::setSwitch(bool ifShown) {
	this->inventoryUI = ifShown;
}


void Picking::update(float msec) {
	GameManager *gameManager = GameManager::getInstance();
	Collider* coll = gameObject->getComponent<Collider>();
	Profiler::getInstance()->startCountingTime();
	GraphNode * object = CollisionSystem::getInstance()->castRay(camera->getGameObject()->worldTransform.getPosition(), camera->getGameObject()->getFrontVector(), distance, coll);
	Profiler::getInstance()->addMeasure("raycast");
	encouragementCanvas->setActive(false);
	bool interactive = false;
	if (object && object->isActive()) {
		CollectableWatch *watch = object->getComponent<CollectableWatch>();
		if (watch && watch->isComponentActive()) {
			interactive = true;
			encouragementCanvas->setActive(true);
			encouragementActivate->setActive(true);
			encouragementPick->setActive(false);
			encouragementActivate->setText("Click to take the watch");
			if (gameManager->getMouseOnce(GLFW_MOUSE_BUTTON_LEFT)) {
				watch->pickup();
				collect(watch->getCollectable());
				previewCanvas->addChild(watch->getCollectable()->getPreview());
				previewCanvas->setActive(true);
			}
		} else {
			CollectableObject *collectable = object->getComponent<CollectableObject>();
			if (collectable && collectable->isComponentActive()) {
				interactive = true;
				encouragementCanvas->setActive(true);
				encouragementActivate->setActive(false);
				encouragementPick->setActive(true);

				if (gameManager->getMouseOnce(GLFW_MOUSE_BUTTON_LEFT) && !collectable->getIsTaken()) {
					collect(collectable);
				}
			}
		}

		Animation* anim = object->getComponent<Animation>();
		AnimationController* animController = object->getComponent<AnimationController>();

		if (anim && anim->isComponentActive() && !anim->getIsPlaying() && !animController) {
			interactive = true;
			encouragementCanvas->setActive(true);
			encouragementActivate->setActive(true);
			encouragementPick->setActive(false);
			encouragementActivate->setText("Click to interact");

			if (gameManager->getMouseOnce(GLFW_MOUSE_BUTTON_LEFT)) {
				anim->play();
                if(object->getComponent<AnimTimeSaver>()) {
                    removeUnusedItem(object->getComponent<AnimTimeSaver>()->getID());
                }
			}
		}

		if (animController && animController->isComponentActive()) {
			interactive = true;
			encouragementCanvas->setActive(true);
			encouragementActivate->setActive(true);
			encouragementPick->setActive(false);

			if (gameManager->getMouseOnce(GLFW_MOUSE_BUTTON_LEFT)) {
				encouragementActivate->setText("Oops.. I need a key");
				gameManager->setCursorLocked(false);
				setSwitch(!getSwitch());
				currentCanvas = itemsInventoryCanvas;
				showInventoryUi();
				currentInteraction = object;
			}
		}
	}
	if(!interactive) {
		object = nullptr;
	}
	if (shiningObject != object) {
		if (shiningObject != nullptr) {
			if (shiningObject->getMesh() != nullptr) {
				shiningObject->getMesh()->setEmissiveFactor(0.0f);
			}
			shiningObject = nullptr;
		}
		if (object != nullptr) {
			shiningObject = object;
			if (object->getMesh() != nullptr) {
				object->getMesh()->setEmissiveFactor(0.25f);
			}
		}
	}
}

void Picking::removeUnusedItem(int ID) {
    for(int i=0; i<inventory.size();i++) {
		CollectableObject * obj = inventory[i]->getComponent<CollectableObject>();
    	if(obj->getPhotoID() == ID) {
            inventory.erase(inventory.begin() + i);
			photosInventoryCanvas->removeChild(obj->getIcon());
			if(obj->getPreview()) {
				photosInventoryCanvas->removeChild(obj->getPreview());
			}
        }
    }
	//showInventoryUi();
}

void Picking::setShowHint(bool showHint) {
	this->showHint = showHint;
}

bool Picking::getShowHint() {
	return showHint;
}

void Picking::renderGui() {
	Component::renderGui();
	if (active) {
		float dist = distance;

		ImGui::SliderFloat("Picking distance", &dist, 0.1f, 2.0f);
		if(dist != distance)
		{
			distance = dist;
		}
		bool hint = showHint;
		ImGui::Checkbox("Show hint", &hint);
		if (hint != showHint) {
			setShowHint(hint);
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
	root["distance"] = distance;
	return root;
}

void Picking::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	camera = dynamic_cast<Camera*>(serializer->deserialize(root["camera"]).object);
	scene = dynamic_cast<Scene*>(serializer->deserialize(root["scene"]).object);
	distance = root.get("distance", distance).asFloat();
	initialize();
}
Scene *Picking::getScene() const {
	return scene;
}

void Picking::setCamera(Camera* camera) {
	this->camera = camera;
}

Camera* Picking::getCamera() {
	return camera;
}
