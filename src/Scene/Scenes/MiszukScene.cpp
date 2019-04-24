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
#include "Scene/Components/PlayerMovement.h"
#include "Scene/Components/CollectableObject.h"
#include "Scene/Components/Picking.h"

MiszukScene::MiszukScene() {
	GameManager::getInstance()->setCursorLocked(true);
	uboLights = assetManager->getUboLights();
	uboTextureColor = assetManager->getUboTextureColor();
	uboViewProjection = assetManager->getUboViewProjection();

	lightManager->recreateLights(0, 0, 0);
	uboLights->inject(1.0f, 0, 0, 0, 0, 0, nullptr, nullptr, nullptr);
	camera = new Camera();
	updatableShaders.push_back(assetManager->getShader(STModel));
	updatableShaders.push_back(assetManager->getShader(STModelInstanced));
	updatableShaders.push_back(assetManager->getShader(STTexture));
	updatableShaders.push_back(assetManager->getShader(STColor));
	updatableShaders.push_back(assetManager->getShader(STReflect));
	updatableShaders.push_back(assetManager->getShader(STRefract));

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
	animatedBoxNode->addComponent(new BoxCollider(animatedBoxNode, DYNAMIC, false, glm::vec3(0), glm::vec3(1)));
	//-------------

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
	GraphNode* player = new GraphNode(playerMesh, rootNode);
	player->addComponent(new SphereCollider(player, DYNAMIC));
	player->addComponent(new PlayerMovement(player, camera, this));
	player->localTransform.setPosition(glm::vec3(-5.0f, 0.0f, -3.0f));
	//player->addComponent(new PhysicalObject(player));
	player->addComponent(new Picking(player, "picking", camera));

	GraphNode* fallingBoxNode = new GraphNode(fallingBox, rootNode);
	fallingBoxNode->addComponent(new SphereCollider(fallingBoxNode, STATIC));
	fallingBoxNode->addComponent(new PhysicalObject(fallingBoxNode));
	boxNode->localTransform.translate(glm::vec3(4.0f, 3.0f, 2.5f));
	boxNode->localTransform.rotate(130.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	boxNode2->localTransform.setPosition(7.0f, 3.0f, 3.0f);
	sphereNode2->localTransform.translate(glm::vec3(-2.0f, 0.0f, 0.0f));
	simpleBox1->localTransform.setPosition(0.0f, 2.0f, 0.0f);
	planete->localTransform.setPosition(7.0f, 3.0f, 0.0f);
	simpleBox2->localTransform.setPosition(0.0f, 0.0f, 1.0f);
	floor->localTransform.setPosition(0.0f, -3.0f, 0.0f);
	fallingBoxNode->localTransform.setPosition(0.0f, -1.5f, -3.0f);

	wallNode->localTransform.translate(glm::vec3(8.0f, -2.0f, -5.0f));
	wallNode2->localTransform.translate(glm::vec3(8.0f, -2.0f, -15.0f));
	slidingDoorNode->localTransform.translate(glm::vec3(8.0f, -2.0f, -6.0f));
	animatedBoxNode->localTransform.translate(glm::vec3(8.0f, -2.0f, 0.0f));

	slidingDoorNode->addComponent(new AnimationController(DoorOpeningX, slidingDoorNode));
	slidingDoorNode->addComponent(new BoxCollider(slidingDoorNode, STATIC, true, glm::vec3(0, 1.0f, 0), glm::vec3(0.5f, 1.0f, 0.5f)));
	boxNode2->addComponent(new SphereCollider(boxNode2, DYNAMIC, true, glm::vec3(-0.5f, 0.0f, 0.0f), 1.0f));
	//boxNode3->addComponent(new AnimationController());
	boxNode->addComponent(new BoxCollider(boxNode, DYNAMIC, true, glm::vec3(1, 0, 0), glm::vec3(1.3f, 1.0f, 0.5f)));
	boxNode->addComponent(new CollisionTest(boxNode));
	simpleBox1->addComponent(new BoxCollider(simpleBox1, STATIC, false, glm::vec3(0, 0, 0), glm::vec3(0.5f, 1.0f, 0.5f)));
	simpleBox2->addComponent(new BoxCollider(simpleBox2, STATIC, false, glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.5f, 0.5f)));
	pivot->addComponent(new BoxCollider(pivot, DYNAMIC, true, glm::vec3(7.0f, 3.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.5f)));
	pivot->addComponent(new CollisionTest(pivot));

	floor->addComponent(new BoxCollider(floor, STATIC, false, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.5f, 10.0f)));
	//simpleBox2->localTransform.setPosition(0.5f, 2.0f, 0.0f);
	rootNode->updateDrawData();
	reinitializeRenderMap();
}

MiszukScene::~MiszukScene() {
	delete rootNode;
}

void MiszukScene::render() {
	for (auto &shader : updatableShaders) {
		shader->use();
		shader->setViewPosition(camera->getPos());
	}
	uboViewProjection->inject(camera->getView(), projection);
	rootNode->updateDrawData();
	renderNodesUsingRenderMap();
	//OctreeNode::getInstance()->draw();
}

void MiszukScene::renderUi() {
	Scene::renderUi();
	assetManager->getTextRenderer()->renderText(assetManager->getShader(STText), "+", gameManager->getScreenWidth() / 2, gameManager->getScreenHeight() / 2, 1.0f);
}

Camera* MiszukScene::getCamera() {
	return camera;
}

void MiszukScene::update(double deltaTime) {

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

	if (abs(mouseMovementX) < 1000.0f) {
		camera->rotateX(mouseMovementX * 0.06f);
	}
	if (abs(mouseMovementY) < 1000.0f) {
		camera->rotateY(-mouseMovementY * 0.06f);
	}
	mouseMovementX = 0.0f;
	mouseMovementY = 0.0f;

	rootNode->update(deltaTime);
	GraphNode* node = camera->castRayFromCamera(camera->getFront(), 3.0f);
	if(node != nullptr)
	{
		//std::cout << "Game object hit" << std::endl;
	}
	else
	{
		//std::cout << "ray casted" << std::endl;
	}
	//std::cout << " Frustum: " << OctreeNode::frustumContainer.size() << " Octree: " << OctreeNode::toInsert2.size() << std::endl;

	OctreeNode::getInstance()->RebuildTree(15.0f);
	OctreeNode::getInstance()->Calculate();
	OctreeNode::getInstance()->CollisionTests();
	camera->RecalculateFrustum();
	Frustum frustum = camera->getFrustum();
	OctreeNode::getInstance()->frustumCulling(frustum);
}

void MiszukScene::mouse_callback(GLFWwindow * window, double xpos, double ypos) {
	Scene::mouse_callback(window, xpos, ypos);
	if (initMouse) {
		mouseX = xpos;
		mouseY = ypos;
		initMouse = false;
	} else {
		mouseMovementX += xpos - mouseX;
		mouseMovementY += ypos - mouseY;
		mouseX = xpos;
		mouseY = ypos;
	}
}

void MiszukScene::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	Scene::updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);
	projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.1f, 100.0f);
}

void MiszukScene::keyEvent(int key, bool pressed) {
	switch (key) {
		case KEY_INTERACT:
			if(pressed) {
				f_keyPressed = true;
			}
			else {
				f_keyPressed = false;
			}
			break;
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
		case KEY_QUIT:
		{
			gameManager->goToMenu(false);
		}
		break;
	}
}
