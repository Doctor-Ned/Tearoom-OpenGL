#include "TestScene.h"
#include "Mesh/MeshColorPlane.h"
#include "SceneManager.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/Model.h"
#include "DirLightNode.h"
#include "RotatingNode.h"
#include "SpotLightNode.h"
#include "PointLightNode.h"

TestScene::TestScene() {
	depthShader = sceneManager->getShader(STDepth);
	depthPointShader = static_cast<GeometryShader*>(sceneManager->getShader(STDepthPoint));

	uboLights = sceneManager->getUboLights();
	uboTextureColor = sceneManager->getUboTextureColor();
	uboViewProjection = sceneManager->getUboViewProjection();
	MeshColorPlane *plane = new MeshColorPlane(10.0f, 10.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	MeshColorSphere *sphere = new MeshColorSphere(0.125f, 30, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	GraphNode* planeNode = new GraphNode(plane);
	GraphNode* sphereNode = new GraphNode(sphere);
	sphereNode->setLocal(translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -1.0f)));
	planeNode->setLocal(rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
	rootNode->addChild(planeNode);
	rootNode->addChild(sphereNode);

	GraphNode *rotatingNode = new RotatingNode(0.01f, nullptr, rootNode);

	DirLight dirLight;
	dirLight.specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	dirLight.diffuse = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight.direction = normalize(glm::vec4(0.0f, -1.0f, -1.0f, 1.0f));
	dirLight.model = glm::mat4(1.0f);
	dirLightNode = new DirLightNode(&dirLight, nullptr, rotatingNode);
	//dirLightNode->setLocal(translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 3.0f))); ??

	GraphNode *rotatingNode2 = new RotatingNode(0.075f, nullptr, rootNode);

	SpotLight spotLight;
	spotLight.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	spotLight.diffuse = glm::vec4(0.0f, 0.0f, 0.6f, 1.0f);
	spotLight.specular = glm::vec4(0.0f, 0.0f, 0.6f, 1.0f);
	spotLight.position = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	spotLight.direction = normalize(glm::vec4(1.0f, -1.0f, 0.0f, 1.0f));
	spotLight.model = glm::mat4(1.0f);
	spotLight.constant = 0.18f;
	spotLight.linear = 0.1f;
	spotLight.quadratic = 0.1f;
	spotLight.cutOff = glm::radians(12.5f);
	spotLight.outerCutOff = glm::radians(25.0f);
	spotLightNode = new SpotLightNode(&spotLight, nullptr, rotatingNode2);

	spotLightProjection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

	GraphNode *rotatingNode3 = new RotatingNode(0.15f, nullptr, rootNode);

	PointLight pointLight;
	pointLight.model = glm::mat4(1.0f);
	pointLight.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	pointLight.diffuse = glm::vec4(0.3f, 0.3f, 0.1f, 1.0f);
	pointLight.specular = glm::vec4(0.3f, 0.3f, 0.1f, 1.0f);
	pointLight.position = glm::vec4(0.0f, 0.5f, 2.0f, 1.0f);
	pointLight.model = glm::mat4(1.0f);
	pointLight.constant = 0.18f;
	pointLight.linear = 0.1f;
	pointLight.quadratic = 0.1f;
	pointLight.near_plane = 0.1f;
	pointLight.far_plane = 10.0f;
	pointLightNode = new PointLightNode(&pointLight, nullptr, rotatingNode3);

	dirLights.push_back(dirLight);
	spotLights.push_back(spotLight);
	pointLights.push_back(pointLight);

	dirLightShadows = Global::getDirsShadowData(dirLights.size());
	spotLightShadows = Global::getSpotsShadowData(spotLights.size());
	pointLightShadows = Global::getPointsShadowData(pointLights.size());

	dirLightNodes.push_back(dirLightNode);
	spotLightNodes.push_back(spotLightNode);
	pointLightNodes.push_back(pointLightNode);

	camera = new Camera();
	updatableShaders.push_back(sceneManager->getShader(STModel));
	updatableShaders.push_back(sceneManager->getShader(STModelInstanced));
	updatableShaders.push_back(sceneManager->getShader(STTexture));
	updatableShaders.push_back(sceneManager->getShader(STColor));
	updatableShaders.push_back(sceneManager->getShader(STReflect));
	updatableShaders.push_back(sceneManager->getShader(STRefract));
}

void TestScene::render() {
	renderDirLights();
	renderSpotLights();
	renderPointLights();

	uboLights->inject(0.05f, dirLights.size(), spotLights.size(), pointLights.size(), &dirLights[0], &spotLights[0], &pointLights[0]);

	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto &shader : updatableShaders) {
		shader->setViewPosition(camera->getPos());
		shader->updateShadowData(dirLightShadows, spotLightShadows, pointLightShadows);
	}
	uboViewProjection->inject(camera->getView(), projection);
	rootNode->draw();
	for (auto &elem : uiElements) {
		elem->render();
	}
	sceneManager->getTextRenderer()->renderText("dupa", 0, 0, 1.0f, false);
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
		camera->rotateX(mouseMovementX * deltaTime);
	}
	if (abs(mouseMovementY) < 1000.0f) {
		camera->rotateY(-mouseMovementY * deltaTime);
	}
	mouseMovementX = 0.0f;
	mouseMovementY = 0.0f;

	rootNode->update(deltaTime);
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

void TestScene::mouse_button_callback(GLFWwindow * window, int butt, int action, int mods) {}

void TestScene::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	Scene::updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);
	projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.1f, 100.0f);
}

void TestScene::renderDirLights() {
	for (int i = 0; i < dirLights.size(); i++) {
		LightShadowData data = dirLightShadows[i];
		DirLight light = dirLights[i];
		DirLightNode *node = dirLightNodes[i];

		glBindFramebuffer(GL_FRAMEBUFFER, data.fbo);
		glViewport(0, 0, data.width, data.height);
		glClear(GL_DEPTH_BUFFER_BIT);
		depthShader->use();
		glm::vec3 position = glm::vec3(node->getWorld()[3]);
		glm::mat4 projection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.1f, 100.0f);
		light.lightSpace = projection * lookAt(position - normalize(glm::vec3(node->getWorld() * glm::vec4(glm::vec3(light.direction), 0.0f))), position, glm::vec3(0.0f, 1.0f, 0.0f));
		depthShader->setLightSpace(light.lightSpace);
		rootNode->draw(depthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, sceneManager->getFramebuffer());
	}
}

void TestScene::renderSpotLights() {
	for (int i = 0; i < spotLights.size(); i++) {
		LightShadowData data = spotLightShadows[i];
		SpotLight light = spotLights[i];
		SpotLightNode *node = spotLightNodes[i];

		glBindFramebuffer(GL_FRAMEBUFFER, data.fbo);
		glViewport(0, 0, data.width, data.height);
		glClear(GL_DEPTH_BUFFER_BIT);
		depthShader->use();
		glm::mat4 world = node->getWorld() * translate(glm::mat4(1.0f), glm::vec3(light.position));
		glm::vec3 pos = world[3];
		light.lightSpace = spotLightProjection * lookAt(pos, pos + glm::normalize(glm::vec3(world * glm::vec4(glm::vec3(light.direction), 0.0f))), glm::vec3(0.0f, 1.0f, 0.0f));
		depthShader->setLightSpace(light.lightSpace);
		rootNode->draw(depthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, sceneManager->getFramebuffer());
	}
}

void TestScene::renderPointLights() {
	for (int i = 0; i < pointLights.size(); i++) {
		LightShadowData data = pointLightShadows[i];
		PointLight light = pointLights[i];
		PointLightNode *node = pointLightNodes[i];

		glBindFramebuffer(GL_FRAMEBUFFER, data.fbo);
		glViewport(0, 0, data.width, data.height);
		glClear(GL_DEPTH_BUFFER_BIT);
		depthPointShader->setFloat("near_plane", light.near_plane);
		depthPointShader->setFloat("far_plane", light.far_plane);
		glm::mat4 world = node->getWorld() * translate(glm::mat4(1.0f), glm::vec3(light.position));
		glm::vec3 position = world[3];
		depthPointShader->setPointPosition(position);
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, light.near_plane, light.far_plane);

		glm::mat4 pointSpaces[6];

		static glm::vec3 targets[6] = {
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f)
		};
		static glm::vec3 ups[6] = {
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f)
		};

		for(int i=0;i<6;i++) {
			pointSpaces[i] = projection * lookAt(position, position + targets[i], ups[i]);
		}
		depthPointShader->setPointSpaces(pointSpaces);
		rootNode->draw(depthPointShader);
		glBindFramebuffer(GL_FRAMEBUFFER, sceneManager->getFramebuffer());
	}
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
	}
}
