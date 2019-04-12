#include "TestScene.h"
#include "Mesh/MeshColorPlane.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/Model.h"
#include "Scene/DirLightNode.h"
#include "Scene/RotatingNode.h"
#include "Scene/SpotLightNode.h"
#include "Scene/PointLightNode.h"
#include "Render/LightManager.h"

TestScene::TestScene() {
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
	modelNode->localTransform.SetMatrix(scale(translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f)), glm::vec3(0.01, 0.01, 0.01f)));

	MeshColorPlane *plane = new MeshColorPlane(10.0f, 10.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	MeshColorSphere *sphere = new MeshColorSphere(0.125f, 30, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	GraphNode* planeNode = new GraphNode(plane, rootNode);
	GraphNode* sphereNode = new GraphNode(sphere, rootNode);
	sphereNode->localTransform.SetMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	planeNode->localTransform.SetMatrix(rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)));

	MeshColorSphere *lightSphere = new MeshColorSphere(0.05f, 30, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	lightSphere->setUseLight(false);


	GraphNode *rotatingNode = new RotatingNode(0.01f, nullptr, rootNode);

	lights = lightManager->recreateLights(1, 1, 1);

	DirLight *dirLight = lights.dirLights[0];
	dirLight->specular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	dirLight->ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	dirLight->diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	dirLight->model = glm::mat4(1.0f);
	//dirLight->model = translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 10.0f));
	GraphNode *dirNode = new GraphNode(nullptr, rotatingNode);
	//dirNode->localTransform.SetMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f)));
	//dirLightNode = new DirLightNode(dirLight, lightSphere, dirNode);
	dirLightNode = new DirLightNode(dirLight, lightSphere, dirNode);
	dirLightNode->localTransform.SetMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f)));
	//dirLightNode->localTransform.SetMatrix(rotate(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f)), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

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
	spotLightNode->localTransform.SetMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 3.0f)));
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
	pointLightNode->localTransform.SetMatrix(translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f)));

	dirLightNodes.push_back(dirLightNode);
	spotLightNodes.push_back(spotLightNode);
	pointLightNodes.push_back(pointLightNode);

	camera = new Camera();
	updatableShaders.push_back(assetManager->getShader(STModel));
	updatableShaders.push_back(assetManager->getShader(STModelInstanced));
	updatableShaders.push_back(assetManager->getShader(STTexture));
	updatableShaders.push_back(assetManager->getShader(STColor));
	updatableShaders.push_back(assetManager->getShader(STReflect));
	updatableShaders.push_back(assetManager->getShader(STRefract));
}

void TestScene::render() { 
	lightManager->renderAndUpdate([&rootNode = rootNode](Shader *shader) {
		rootNode->draw(shader, true);
	}, updatableShaders);

	pointLightSphere->setColor(lights.pointLights[0]->diffuse);

	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto &shader : updatableShaders) {
		shader->setViewPosition(camera->getPos());
	}
	uboViewProjection->inject(camera->getView(), projection);

	rootNode->draw();
	OctreeNode::getInstance()->draw();
	skybox->draw(camera->getUntranslatedView(), projection);
}

void TestScene::renderUi() {
	Scene::renderUi();

	if(modelNode != nullptr) {
		static float opacity = 1.0f;
		ImGui::SliderFloat("Model opacity", &opacity, 0.0f, 1.0f);
		modelNode->setOpacity(opacity);
		modelNode->drawGui();
	}

	dirLightNode->getParent()->drawGui();
	dirLightNode->drawGui();
	spotLightNode->drawGui();
	pointLightNode->drawGui();

	ImGui::SliderFloat("Dir near plane", &lightManager->dirNear, 0.01f, 100.0f);
	ImGui::SliderFloat("Dir far plane", &lightManager->dirFar, 0.01f, 100.0f);
	ImGui::SliderFloat("Dir proj size", &lightManager->dirProjSize, 0.01f, 50.0f);
	ImGui::SliderFloat("Spot near plane", &lightManager->spotNear, 0.01f, 100.0f);
	ImGui::SliderFloat("Spot far plane", &lightManager->spotFar, 0.01f, 100.0f);
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

	rootNode->update(deltaTime);
	OctreeNode::getInstance()->RebuildTree(15.0f);
	OctreeNode::getInstance()->Calculate();
	/*OctreeNode::getInstance()->CollisionTests();
	OctreeNode::getInstance()->castRayFromCamera(camera, 3.0f);*/
}

void TestScene::keyboard_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		if (getKeyState(key)) {
			setKeyState(key, false);
			keyEvent(key, false);
		}
	}
	if (action == GLFW_PRESS) {
		if (!getKeyState(key)) {
			setKeyState(key, true);
			keyEvent(key, true);
		}
	}
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

void TestScene::mouse_button_callback(GLFWwindow * window, int butt, int action, int mods) {
	Scene::mouse_button_callback(window, butt, action, mods);
}

void TestScene::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	Scene::updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);
	projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.1f, 100.0f);
}

bool TestScene::getKeyState(int key) {
	auto pair = keyStates.find(key);
	if (pair != keyStates.end()) {
		return pair->second;
	}
	return false;
}

void TestScene::setKeyState(int key, bool pressed) {
	auto pair = keyStates.find(key);
	if (pair != keyStates.end()) {
		pair->second = pressed;
	} else {
		keyStates.emplace(key, pressed);
	}
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
			//glfwSetWindowShouldClose(gameManager->getWindow(), true);
			break;
	}
}
