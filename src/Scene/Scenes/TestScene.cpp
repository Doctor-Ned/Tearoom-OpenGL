#include "TestScene.h"
#include "Mesh/MeshColorPlane.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/Model.h"
#include "Scene/DirLightNode.h"
#include "Scene/RotatingNode.h"
#include "Scene/SpotLightNode.h"
#include "Scene/PointLightNode.h"
#include "Render/LightManager.h"
#include "Mesh/MeshPlane.h"
#include "Scene/BillboardNode.h"
#include <iostream>
#include "Scene/Scripts/CollisionTest.h"

TestScene::TestScene() {
	camera = new Camera();

	std::vector<std::string> faces;
	//faces.emplace_back("res/skybox/arrakisday/arrakisday_rt.tga");
	//faces.emplace_back("res/skybox/arrakisday/arrakisday_lf.tga");
	//faces.emplace_back("res/skybox/arrakisday/arrakisday_up.tga");
	//faces.emplace_back("res/skybox/arrakisday/arrakisday_dn.tga");
	//faces.emplace_back("res/skybox/arrakisday/arrakisday_ft.tga");
	//faces.emplace_back("res/skybox/arrakisday/arrakisday_bk.tga");
	faces.emplace_back("res/skybox/test/right.jpg");
	faces.emplace_back("res/skybox/test/left.jpg");
	faces.emplace_back("res/skybox/test/top.jpg");
	faces.emplace_back("res/skybox/test/bottom.jpg");
	faces.emplace_back("res/skybox/test/front.jpg");
	faces.emplace_back("res/skybox/test/back.jpg");

	skybox = new Skybox(assetManager->getShader(STSkybox), faces);

	uboLights = assetManager->getUboLights();
	uboTextureColor = assetManager->getUboTextureColor();
	uboViewProjection = assetManager->getUboViewProjection();

	Model *model = new Model("res/models/muro/muro.obj");
	modelNode = new GraphNode(model, rootNode);
	modelNode->localTransform.setMatrix(scale(translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f)), glm::vec3(0.01, 0.01, 0.01f)));

	MeshColorBox *floor = new MeshColorBox(glm::vec3(10.0f, 1.0f, 10.0f), glm::vec4(0.1f, 1.0f, 0.1f, 1.0f));

	//MeshColorPlane *plane = new MeshColorPlane(10.0f, 10.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	//plane->setCulled(false);
	MeshColorSphere *sphere = new MeshColorSphere(0.125f, 30, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	GraphNode* floorNode = new GraphNode(floor, rootNode);
	floorNode->localTransform.translate(glm::vec3(0.0f, -0.5f, 0.0f));
	GraphNode* sphereNode = new GraphNode(sphere, rootNode);
	sphereNode->localTransform.setMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

	MeshColorSphere *lightSphere = new MeshColorSphere(0.05f, 30, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	lightSphere->setUseLight(false);

	//GraphNode *rotatingNode = new RotatingNode(0.01f, nullptr, rootNode);

	lights = lightManager->recreateLights(2, 1, 1);

	sunNode = new SunNode(lights.dirLights[0], lights.dirLights[1],
		normalize(glm::vec4(255.0f, 183.0f, 0.0f, 255.0f)),
		normalize(glm::vec4(255.0f, 210.0f, 120.0f, 255.0f)),
		normalize(glm::vec4(52.0f, 86.0f, 181.0f, 255.0f) * 0.8f),
		normalize(glm::vec4(70.0f, 115.0f, 189.0f, 255.0f) * 0.8f), 10.0f, 12.0f, rootNode);

	//DirLight *dirLight = lights.dirLights[0];
	//dirLight->specular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	//dirLight->ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//dirLight->diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	//dirLight->model = glm::mat4(1.0f);
	//dirLight->model = translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 10.0f));
	//GraphNode *dirNode = new GraphNode(nullptr, rotatingNode);
	//dirNode->localTransform.SetMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f)));
	//dirLightNode = new DirLightNode(dirLight, lightSphere, dirNode);
	//dirLightNode = new DirLightNode(dirLight, lightSphere, dirNode);
	//dirLightNode->localTransform.setMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f)));
	//dirLightNode->localTransform.SetMatrix(rotate(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f)), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

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
	GraphNode* floor1 = new GraphNode(floorMesh, rootNode);

	boxNode->localTransform.translate(glm::vec3(4.0f, 3.0f, 2.5f));
	boxNode->localTransform.rotate(130.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	boxNode2->localTransform.setPosition(7.0f, 3.0f, 3.0f);
	sphereNode2->localTransform.translate(glm::vec3(-2.0f, 0.0f, 0.0f));
	simpleBox1->localTransform.setPosition(0.0f, 2.0f, 0.0f);
	planete->localTransform.setPosition(7.0f, 3.0f, 0.0f);
	simpleBox2->localTransform.setPosition(0.0f, 0.0f, 1.0f);
	floor1->localTransform.setPosition(0.0f, -3.0f, 0.0f);

	boxNode2->addComponent(new SphereCollider(boxNode2, DYNAMIC, true, glm::vec3(-0.5f, 0.0f, 0.0f), 1.0f));

	//boxNode->addComponent(new BoxCollider(boxNode, DYNAMIC, true, glm::vec3(1, 0, 0), glm::vec3(1.3f, 1.0f, 0.5f)));
	//boxNode->addComponent(new CollisionTest(boxNode));
	//simpleBox1->addComponent(new BoxCollider(simpleBox1, STATIC, false, glm::vec3(0, 0, 0), glm::vec3(0.5f, 1.0f, 0.5f)));
	//simpleBox2->addComponent(new BoxCollider(simpleBox2, STATIC, false, glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.5f, 0.5f)));
	//pivot->addComponent(new BoxCollider(pivot, DYNAMIC, true, glm::vec3(7.0f, 3.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.5f)));
	//pivot->addComponent(new CollisionTest(pivot));

	//floor1->addComponent(new BoxCollider(floor1, STATIC, false, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.5f, 10.0f)));


	GraphNode *rotatingNode2 = new RotatingNode(0.075f, nullptr, rootNode);

	SpotLight *spotLight = lights.spotLights[0];
	spotLight->ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	spotLight->diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
	spotLight->specular = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	spotLight->constant = 0.18f;
	spotLight->linear = 0.1f;
	spotLight->quadratic = 0.1f;
	spotLightNode = new SpotLightNode(spotLight, lightSphere, rotatingNode2);
	//GraphNode *spotNode = new GraphNode(nullptr, rotatingNode);
	//spotNode->localTransform.SetMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)));
	spotLightNode->localTransform.setMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 3.0f)));
	//spotLightNode->localTransform.SetMatrix(rotate(translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 3.0f)), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

	GraphNode *rotatingNode3 = new RotatingNode(0.15f, nullptr, rootNode);

	PointLight *pointLight = lights.pointLights[0];
	pointLight->ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	pointLight->diffuse = glm::vec4(0.3f, 0.3f, 0.1f, 1.0f);
	pointLight->specular = glm::vec4(0.3f, 0.3f, 0.1f, 1.0f);
	pointLight->constant = 0.18f;
	pointLight->linear = 0.1f;
	pointLight->quadratic = 0.1f;
	pointLightSphere = new MeshColorSphere(0.125f, 30, pointLight->diffuse);
	pointLightSphere->setShaderType(STLight);
	pointLightNode = new PointLightNode(pointLight, pointLightSphere, rotatingNode3);
	pointLightNode->localTransform.setMatrix(translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f)));

	dirLightNodes.push_back(dirLightNode);
	spotLightNodes.push_back(spotLightNode);
	pointLightNodes.push_back(pointLightNode);

	BillboardNode *billboardNode = new BillboardNode(camera, nullptr, rootNode, true);
	billboardNode->localTransform.setMatrix(translate(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, -3.0f)));
	MeshPlane *emote = new MeshPlane(0.25f, 0.25f, "res/textures/face.png");
	emote->setUseLight(false);
	emote->setOpaque(false);
	GraphNode *emoteNode = new GraphNode(emote, billboardNode);
	emoteNode->localTransform.setMatrix(rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

	updatableShaders.push_back(assetManager->getShader(STModel));
	updatableShaders.push_back(assetManager->getShader(STModelInstanced));
	updatableShaders.push_back(assetManager->getShader(STTexture));
	updatableShaders.push_back(assetManager->getShader(STColor));
	updatableShaders.push_back(assetManager->getShader(STReflect));
	updatableShaders.push_back(assetManager->getShader(STRefract));

	reinitializeRenderMap();
}

void TestScene::render() {
	rootNode->updateDrawData();

	lightManager->renderAndUpdate([this](Shader *shader) {
		renderNodesUsingRenderMap(shader, true);
		renderNodesUsingTransparentRenderMap(shader, true);
	}, updatableShaders);

	pointLightSphere->setColor(lights.pointLights[0]->diffuse);

	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto &shader : updatableShaders) {
		shader->use();
		shader->setViewPosition(camera->getPos());
	}
	uboViewProjection->inject(camera->getView(), projection);

	renderNodesUsingRenderMap();

	//OctreeNode::getInstance()->draw();
	skybox->draw(camera->getUntranslatedView(), projection);

	renderNodesUsingTransparentRenderMap();
}

void TestScene::renderUi() {
	Scene::renderUi();

	static float time = sunNode->getTime();
	ImGui::SliderFloat("Time", &time, -24.0f, 24.0f);
	if (time != sunNode->getTime()) {
		sunNode->setTime(time);
	}

	if (modelNode != nullptr) {
		static float opacity = 1.0f;
		ImGui::SliderFloat("Model opacity", &opacity, 0.0f, 1.0f);
		modelNode->setOpacity(opacity);
		modelNode->drawGui();
	}

	//dirLightNode->getParent()->drawGui();
	//dirLightNode->drawGui();
	spotLightNode->drawGui();
	pointLightNode->drawGui();

	ImGui::SliderFloat("Dir near plane", &lightManager->dirNear, 0.01f, 100.0f);
	ImGui::SliderFloat("Dir far plane", &lightManager->dirFar, 0.01f, 100.0f);
	ImGui::SliderFloat("Dir proj size", &lightManager->dirProjSize, 0.01f, 50.0f);
	ImGui::SliderFloat("Spot near plane", &lightManager->spotNear, 0.01f, 100.0f);
	ImGui::SliderFloat("Spot far plane", &lightManager->spotFar, 0.01f, 100.0f);
}

Camera* TestScene::getCamera() {
	return camera;
}

void TestScene::update(double deltaTime) {

	if (getKeyState(KEY_FORWARD)) {
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
	}
	if (getKeyState(KEY_DOWN)) {
		camera->moveDown(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_MOUSE_LEFT)) {
		camera->rotateX(-movementSpeed * deltaTime * 5.0f);
	}
	if (getKeyState(KEY_MOUSE_RIGHT)) {
		camera->rotateX(movementSpeed * deltaTime * 5.0f);
	}
	if (getKeyState(KEY_MOUSE_UP)) {
		camera->rotateY(movementSpeed * deltaTime * 5.0f);
	}
	if (getKeyState(KEY_MOUSE_DOWN)) {
		camera->rotateY(-movementSpeed * deltaTime * 5.0f);
	}

	if (lockMouse) {
		if (abs(mouseMovementX) < 1000.0f) {
			camera->rotateX(mouseMovementX * 0.06f);
		}
		if (abs(mouseMovementY) < 1000.0f) {
			camera->rotateY(-mouseMovementY * 0.06f);
		}
	}
	mouseMovementX = 0.0f;
	mouseMovementY = 0.0f;

	//sunNode->addTime(deltaTime);
	rootNode->update(deltaTime);
	
	//std::cout << " Frustum: " << OctreeNode::frustumContainer.size() << " Octree: " << OctreeNode::toInsert2.size() << std::endl;
	OctreeNode::getInstance()->RebuildTree(15.0f);
	OctreeNode::getInstance()->Calculate();
	OctreeNode::getInstance()->CollisionTests();
	camera->RecalculateFrustum();
	Frustum frustum = camera->getFrustum();
	OctreeNode::getInstance()->frustumCulling(frustum);
	//OctreeNode::getInstance()->CollisionTests();
}

void TestScene::mouse_callback(GLFWwindow * window, double xpos, double ypos) {
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

void TestScene::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	Scene::updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);
	projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.1f, 100.0f);
}

void TestScene::keyEvent(int key, bool pressed) {
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
				lockMouse = !lockMouse;
				gameManager->setCursorLocked(lockMouse);
				initMouse = true;
			}
			break;
		case KEY_QUIT:
			gameManager->goToMenu();
			break;
	}
}
