#include "MiszukScene.h"
#include "Mesh/MeshColorPlane.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/Model.h"
#include "TestScene.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/OctreeNode.h"
#include <iostream>
#include "Scene/Components/Collider.h"
#include "Scene/Scripts/CollisionTest.h"
#include "Scene/Components/AnimationController.h"
#include "Scene/Components/BoxCollider.h"
#include "Scene/Components/SphereCollider.h"
#include "Scene/Components/PhysicalObject.h"
#include "Scene/Scripts/PlayerMovement.h"
#include "Scene/Scripts/Picking.h"
#include "Scene/Scripts/CollectableObject.h"
#include "Scene/Components/Animation.h"
#include "Ui/UiPlane.h"

MiszukScene::MiszukScene() {
	GameManager::getInstance()->setCursorLocked(true);

	lightManager->recreateLights(0, 0, 0);
	camera = new Camera();

	// for basic animation testing
	MeshColorBox *fallingBox = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	MeshColorBox *animatedBox = new MeshColorBox(glm::vec3(-0.3f, 0.3f, -1), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	MeshColorBox *slidingDoor = new MeshColorBox(glm::vec3(-0.3f, 0.4f, -1), glm::vec3(0.7f, 5.4f, 0.7f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	MeshColorBox *wall = new MeshColorBox(glm::vec3(-0.3f, 0.3f, 0.0f), glm::vec3(0.7f, 5.4f, 8.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	MeshColorBox *wall2 = new MeshColorBox(glm::vec3(-0.3f, 0.3f, 0.0f), glm::vec3(0.7f, 5.4f, 8.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	GraphNode* animatedBoxNode = new GraphNode(animatedBox, rootNode);
	GraphNode* slidingDoorNode = new GraphNode(slidingDoor, rootNode);
	GraphNode* wallNode = new GraphNode(wall, rootNode);
	GraphNode* wallNode2 = new GraphNode(wall2, rootNode);
	animatedBoxNode->addComponent(new AnimationController(SafePullOutY, animatedBoxNode));
	animatedBoxNode->addComponent(new BoxCollider(animatedBoxNode, STATIC, false, glm::vec3(0), glm::vec3(1)));
	
	slidingDoorNode->addComponent(new AnimationController(DoorOpeningX, slidingDoorNode));
	slidingDoorNode->addComponent(new BoxCollider(slidingDoorNode, STATIC, true, glm::vec3(0, 1.0f, 0), glm::vec3(0.5f, 1.0f, 0.5f)));

	slidingDoorNode->localTransform.translate(glm::vec3(8.0f, -2.0f, -6.0f));
	animatedBoxNode->localTransform.translate(glm::vec3(8.0f, -1.0f, 0.0f));

	//-------------
    //INVENTORY UI
    UiPlane* hud = new UiPlane("res/textures/inventory.png",glm::vec2(500.0f, 500.0f), glm::vec2(400.0f,400.0f), Center);
    UiColorPlane* boxRepresentation = new UiColorPlane(glm::vec4(0.0f,0.0f,1.0f,1.0f), glm::vec2(1080.0f, 430.0f),glm::vec2(50.0f, 50.0f), Right);
    UiColorPlane* boxRepresentation2 = new UiColorPlane(glm::vec4(0.0f,0.0f,1.0f,1.0f), glm::vec2(1170.0f, 430.0f),glm::vec2(50.0f, 50.0f), Right);
	boxRepresentation->setParent(rootUiElement);
	boxRepresentation2->setParent(rootUiElement);
    objectRepresentasions.push_back(boxRepresentation);
    objectRepresentasions.push_back(boxRepresentation2);
    inventoryText = new UiText(glm::vec2(1140.0f, 360.0f), glm::vec2(60.0f,30.0f), "Inventory", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);
    inventoryBackground = new UiColorPlane(glm::vec4(0.76f, 0.65f, 0.27f, 0.85f), glm::vec2(1400.0f, 700.0f), glm::vec2(400.0f, 700.0f), Right);
	addToRenderMap(hud);
	// COLLECTABLE ITEM
	MeshColorBox *tinyItem = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(0.0f, 0.5f, 0.5f, 1.0f));
	GraphNode *tinyItemNode = new GraphNode(tinyItem, rootNode);
	tinyItemNode->addComponent(new PhysicalObject(tinyItemNode));
	tinyItemNode->addComponent(new CollectableObject(tinyItemNode, camera));
	tinyItemNode->addComponent(new BoxCollider(tinyItemNode, STATIC, false, glm::vec3(0), glm::vec3(1)));
	tinyItemNode->localTransform.translate(glm::vec3(3.0f, -1.0f, 2.0f));
    MeshColorBox *tinyItem2 = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec4(0.0f, 0.5f, 0.5f, 1.0f));
    GraphNode *tinyItemNode2 = new GraphNode(tinyItem2, rootNode);
	tinyItemNode2->addComponent(new PhysicalObject(tinyItemNode2));
    tinyItemNode2->addComponent(new CollectableObject(tinyItemNode2,camera));
    tinyItemNode2->addComponent(new BoxCollider(tinyItemNode2, DYNAMIC, false, glm::vec3(0), glm::vec3(1)));
    tinyItemNode2->localTransform.translate(glm::vec3(3.0f, -0.5f, 4.0f));
	//-----------------

	//miszuk animation
	GraphNode* doorPivot = new GraphNode(nullptr, rootNode);
	MeshColorBox* doorMesh = new MeshColorBox(glm::vec3(2.0f, 3.0f, 0.2f), glm::vec4(0.8f, 1.0f, 1.0f, 1.0f));
	MeshColorBox* handleMesh = new MeshColorBox(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec4(0.8f, 0.0f, 0.0f, 1.0f));
	doorPivot->setName("doorPivot");
	GraphNode* door = new GraphNode(doorMesh, doorPivot);
	door->setName("door");
	GraphNode* handle = new GraphNode(handleMesh, door);
	handle->setName("handle");

	door->addComponent(new BoxCollider(door, STATIC, false, glm::vec3(0), glm::vec3(1.0f, 1.5f, 0.1f)));
	Animation* anim = new Animation(door, "doorOpening");
	anim->addKeyFrame("door", anim::TRANSLATION, 0.0f, glm::vec3(0));
	anim->addKeyFrame("door", anim::TRANSLATION, 1.0f, glm::vec3(0.4f, 0.0f, 0.0f));
	anim->addKeyFrame("door", anim::TRANSLATION, 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	door->addComponent(anim);
	doorPivot->localTransform.setPosition(-4.0f, -1.0f, -2.0f);
	doorPivot->localTransform.rotateYDegrees(180.0f);


	anim->addKeyFrame("handle", anim::TRANSLATION, 2.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
	anim->addKeyFrame("handle", anim::TRANSLATION, 0.0f, glm::vec3(0));
	anim->addKeyFrame("handle", anim::TRANSLATION, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	anim->addKeyFrame("handle", anim::TRANSLATION, 3.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	anim->addKeyFrame("handle", anim::TRANSLATION, 3.0f, glm::vec3(5.0f, 2.0f, 0.0f));
	anim->addKeyFrame("handle", anim::TRANSLATION, 4.0f, glm::vec3(0));

	anim->addKeyFrame("handle", anim::SCALE, 0.0f, glm::vec3(1));
	anim->addKeyFrame("handle", anim::SCALE, 1.0f, glm::vec3(2.0f, 0.5f, 1.0f));
	anim->addKeyFrame("handle", anim::SCALE, 2.0f, glm::vec3(3.0f, 0.5f, 0.5f));
	anim->addKeyFrame("handle", anim::SCALE, 4.0f, glm::vec3(1));

	anim->addKeyFrame("handle", anim::ROTATION, 0.0f, glm::vec3(0));
	anim->addKeyFrame("handle", anim::ROTATION, 2.0f, glm::vec3(90.0f, 90.0f, 180.0f));
	anim->addKeyFrame("handle", anim::ROTATION, 4.0f, glm::vec3(0.0f, 0.0f, 360.0f));
	//


	MeshColorBox* box = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	MeshColorBox* box1 = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
	MeshColorBox* box2 = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	MeshColorBox* floorMesh = new MeshColorBox(glm::vec3(-10.0f, -0.5f, -10.5f), glm::vec3(10.0f, 0.5f, 10.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	Model* sphere2 = new Model("res/models/sphere/sphere.obj");
	MeshColorBox* meshBox = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	MeshColorBox* meshBox1 = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	GraphNode* boxNode = new GraphNode(box, rootNode);
	GraphNode* boxNode2 = new GraphNode(box1, rootNode);
	GraphNode* sphereNode2 = new GraphNode(sphere2, rootNode);
	GraphNode* simpleBox1 = new GraphNode(meshBox, rootNode);
	GraphNode* simpleBox2 = new GraphNode(meshBox1, rootNode);
	GraphNode* pivot = new GraphNode(nullptr, rootNode);
	GraphNode* planete = new GraphNode(box2, pivot);
	GraphNode* floor = new GraphNode(floorMesh, rootNode);

	Mesh* playerMesh = new MeshColorBox(glm::vec3(1.0f, 2.0f, 1.0f), glm::vec4(1));
	player = new GraphNode(playerMesh, rootNode);
	player->addComponent(new BoxCollider(player, DYNAMIC));
	player->addComponent(new PlayerMovement(player, camera, this));
	player->localTransform.setPosition(glm::vec3(-5.0f, 0.0f, -3.0f));
	//player->addComponent(new PhysicalObject(player));
	player->addComponent(new Picking(player, "picking", camera, this));

	GraphNode* fallingBoxNode = new GraphNode(fallingBox, rootNode);
	fallingBoxNode->addComponent(new SphereCollider(fallingBoxNode, STATIC));
	fallingBoxNode->addComponent(new PhysicalObject(fallingBoxNode));
	boxNode->localTransform.translate(glm::vec3(4.0f, 3.0f, 2.5f));
	boxNode->localTransform.rotateYDegrees(130.0f);
	//boxNode->localTransform.rotate(130.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	boxNode2->localTransform.setPosition(7.0f, 3.0f, 3.0f);
	sphereNode2->localTransform.translate(glm::vec3(-2.0f, 0.0f, 0.0f));
	simpleBox1->localTransform.setPosition(0.0f, 2.0f, 0.0f);
	planete->localTransform.setPosition(7.0f, 3.0f, 0.0f);
	simpleBox2->localTransform.setPosition(0.0f, 0.0f, 1.0f);
	floor->localTransform.setPosition(0.0f, -3.0f, 0.0f);
	fallingBoxNode->localTransform.setPosition(0.0f, -1.5f, -3.0f);

	wallNode->localTransform.translate(glm::vec3(8.0f, -2.0f, -5.0f));
	wallNode2->localTransform.translate(glm::vec3(8.0f, -2.0f, -15.0f));
	
	boxNode2->addComponent(new SphereCollider(boxNode2, DYNAMIC, true, glm::vec3(-0.5f, 0.0f, 0.0f), 1.0f));
	//boxNode3->addComponent(new AnimationController());
	boxNode->addComponent(new BoxCollider(boxNode, DYNAMIC, true, glm::vec3(1, 0, 0), glm::vec3(1.3f, 1.0f, 0.5f)));
	boxNode->addComponent(new CollisionTest(boxNode));
	simpleBox1->addComponent(new BoxCollider(simpleBox1, STATIC, false, glm::vec3(0, 0, 0), glm::vec3(0.5f, 1.0f, 0.5f)));
	simpleBox2->addComponent(new BoxCollider(simpleBox2, STATIC, false, glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.5f, 0.5f)));
	simpleBox2->setName("animatedObject");
	//simpleBox2->addComponent(new Animation(simpleBox2, "Animation"));
	pivot->addComponent(new BoxCollider(pivot, DYNAMIC, true, glm::vec3(7.0f, 3.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.5f)));
	pivot->addComponent(new CollisionTest(pivot));

	floor->addComponent(new BoxCollider(floor, STATIC, false, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.5f, 10.0f)));
	//simpleBox2->localTransform.setPosition(0.5f, 2.0f, 0.0f);
	rootNode->updateDrawData();
	rootUiElement->updateDrawData();
	reinitializeRenderMap();
}

MiszukScene::~MiszukScene() {
	delete rootNode;
}

void MiszukScene::render() {
	Scene::render();
	//OctreeNode::getInstance()->draw();
}

void MiszukScene::renderUi() {
	Scene::renderUi();
	assetManager->getTextRenderer()->renderText(assetManager->getShader(STText), "+", gameManager->getScreenWidth() / 2.0f, gameManager->getScreenHeight() / 2.0f, 1.0f);
}

void MiszukScene::update(double deltaTime) {
	Scene::update(deltaTime);

	/*if (getKeyState(KEY_FORWARD)) {
		camera->moveForward(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_BACKWARD)) {
		camera->moveBackward(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_LEFT)) {
		camera->moveLeft(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_RIGHT)) {
		camera->moveRight(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_UP)) {
		camera->moveUp(deltaTime * movementSpeed);
	}*/
	if (getKeyState(KEY_DOWN)) {
		camera->moveDown(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_MOUSE_LEFT)) {
		camera->rotateX(-movementSpeed * deltaTime);
	}
	if (getKeyState(KEY_MOUSE_RIGHT)) {
		camera->rotateX(movementSpeed * deltaTime);
	}
	if (getKeyState(KEY_MOUSE_UP)) {
		camera->rotateY(-movementSpeed * deltaTime);
	}
	if (getKeyState(KEY_MOUSE_DOWN)) {
		camera->rotateY(-movementSpeed * deltaTime);
	}

	rootNode->update(deltaTime);
	//GraphNode* node = camera->castRayFromCamera(camera->getFront(), 3.0f);
	//if(node != nullptr)
	//{
	//	//std::cout << "Game object hit" << std::endl;
	//}
	//else
	//{
	//	//std::cout << "ray casted" << std::endl;
	//}
	//std::cout << " Frustum: " << OctreeNode::frustumContainer.size() << " Octree: " << OctreeNode::toInsert2.size() << std::endl;

	OctreeNode::getInstance()->RebuildTree(15.0f);
	OctreeNode::getInstance()->Calculate();
	OctreeNode::getInstance()->CollisionTests();
	camera->RecalculateFrustum();
	Frustum frustum = camera->getFrustum();
	OctreeNode::getInstance()->frustumCulling(frustum);
}

void MiszukScene::keyEvent(int key, bool pressed) {
	switch (key) {
		case KEY_FAST:
			if (pressed) {
				movementSpeed *= 2.0f;
			} else {
				movementSpeed /= 2.0f;
			}
			break;
		case KEY_SLOW:
			if (!pressed) {
				movementSpeed *= 2.0f;
			} else {
				movementSpeed /= 2.0f;
			}
			break;
		case KEY_TOGGLE_MOUSE_LOCK:
			if (pressed) {
				setCursorLocked(!getCursorLocked());
			}
			break;
		case KEY_QUIT: {
			gameManager->goToMenu(false);
		}
			break;
		case GLFW_KEY_I: {
			Picking *temp = player->getComponent<Picking>();

			if (pressed) {
				if (temp->getSwitch() == false) temp->setSwitch(true);
				else temp->setSwitch(false);
			} else {
				if (temp->getSwitch() == true) {
					temp->showInventoryUi();
				} else {
					temp->hideInventoryUi();
				}

			}
		}
	}
}

