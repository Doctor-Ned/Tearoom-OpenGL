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
	std::vector<std::string> faces;
	faces.emplace_back("res/skybox/arrakisday/arrakisday_rt.tga");
	faces.emplace_back("res/skybox/arrakisday/arrakisday_lf.tga");
	faces.emplace_back("res/skybox/arrakisday/arrakisday_up.tga");
	faces.emplace_back("res/skybox/arrakisday/arrakisday_dn.tga");
	faces.emplace_back("res/skybox/arrakisday/arrakisday_ft.tga");
	faces.emplace_back("res/skybox/arrakisday/arrakisday_bk.tga");

	skybox = new Skybox(sceneManager->getShader(STSkybox), faces);
	depthShader = sceneManager->getShader(STDepth);
	depthDebugShader = sceneManager->getShader(STDepthDebug);
	depthPointShader = static_cast<GeometryShader*>(sceneManager->getShader(STDepthPoint));

	uboLights = sceneManager->getUboLights();
	uboTextureColor = sceneManager->getUboTextureColor();
	uboViewProjection = sceneManager->getUboViewProjection();
	MeshColorPlane *plane = new MeshColorPlane(10.0f, 10.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	MeshColorSphere *sphere = new MeshColorSphere(0.125f, 30, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	GraphNode* planeNode = new GraphNode(plane, rootNode);
	GraphNode* sphereNode = new GraphNode(sphere, rootNode);
	sphereNode->localTransform.SetMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	planeNode->localTransform.SetMatrix(rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)));

	MeshColorSphere *lightSphere = new MeshColorSphere(0.05f, 30, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	lightSphere->setUseLight(false);


	GraphNode *rotatingNode = new RotatingNode(0.01f, nullptr, rootNode);

	DirLight *dirLight = new DirLight();
	dirLight->specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight->ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	dirLight->diffuse = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight->direction = normalize(glm::vec4(0.0f, -1.0f, -1.0f, 1.0f));
	dirLight->model = glm::mat4(1.0f);
	//dirLight->model = translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 10.0f));
	GraphNode *dirNode = new GraphNode(nullptr, rotatingNode);
	dirNode->localTransform.SetMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 10.0f)));
	dirLightNode = new DirLightNode(dirLight, lightSphere, dirNode);
	//dirLightNode->localTransform.SetMatrix(dirLight->model);
	//GraphNode *light1 = new GraphNode(lightSphere, dirLightNode);
	//light1->setLocal(dirLight->model);

	GraphNode *rotatingNode2 = new RotatingNode(0.075f, nullptr, rootNode);

	SpotLight *spotLight = new SpotLight();
	spotLight->ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	spotLight->diffuse = glm::vec4(0.0f, 0.0f, 0.6f, 1.0f);
	spotLight->specular = glm::vec4(0.0f, 0.0f, 0.6f, 1.0f);
	spotLight->direction = normalize(glm::vec4(1.0f, -1.0f, 0.0f, 1.0f));
	spotLight->model = glm::mat4(1.0f);
	spotLight->constant = 0.18f;
	spotLight->linear = 0.1f;
	spotLight->quadratic = 0.1f;
	spotLight->cutOff = glm::radians(12.5f);
	spotLight->outerCutOff = glm::radians(25.0f);
	spotLightNode = new SpotLightNode(spotLight, lightSphere, rotatingNode2);
	spotLightNode->localTransform.SetMatrix(translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 3.0f)));

	GraphNode *rotatingNode3 = new RotatingNode(0.15f, nullptr, rootNode);

	PointLight *pointLight = new PointLight();
	pointLight->model = glm::mat4(1.0f);
	pointLight->ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	pointLight->diffuse = glm::vec4(0.3f, 0.3f, 0.1f, 1.0f);
	pointLight->specular = glm::vec4(0.3f, 0.3f, 0.1f, 1.0f);
	pointLight->model = glm::mat4(1.0f);
	pointLight->constant = 0.18f;
	pointLight->linear = 0.1f;
	pointLight->quadratic = 0.1f;
	pointLight->near_plane = 0.01f;
	pointLight->far_plane = 10.0f;
	pointLightNode = new PointLightNode(pointLight, lightSphere, rotatingNode3);
	pointLightNode->localTransform.SetMatrix(translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.2f, 0.0f)));

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

	uboLights->inject(BASE_AMBIENT, dirLights.size(), spotLights.size(), pointLights.size(), &dirLights[0], &spotLights[0], &pointLights[0]);

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


	dirLightNode->drawGui();
	spotLightNode->drawGui();
	pointLightNode->drawGui();

	ImGui::SliderFloat("Dir near plane", &dirNear, 0.01f, 100.0f);
	ImGui::SliderFloat("Dir far plane", &dirFar, 0.01f, 100.0f);
	ImGui::SliderFloat("Dir proj size", &dirProjSize, 0.01f, 50.0f);
	ImGui::SliderFloat("Spot near plane", &spotNear, 0.01f, 100.0f);
	ImGui::SliderFloat("Spot far plane", &spotFar, 0.01f, 100.0f);

	ImGui::SliderInt("Depth map", &renderDepthMap, 0, 3);
	switch (renderDepthMap) {
		case 0:
			ImGui::Text("None");
			break;
		case 1:
			ImGui::Text("Directional light depth map");
			break;
		case 2:
			ImGui::Text("Spot light depth map");
			break;
		case 3:
			ImGui::Text("Point light depth map");
			break;
	}

	if (renderDepthMap == 3) {
		skybox->draw(camera->getUntranslatedView(), projection, pointLightShadows[0]->texture);
	} else {
		skybox->draw(camera->getUntranslatedView(), projection);
	}

	for (auto &elem : uiElements) {
		elem->render();
	}

	sceneManager->getTextRenderer()->renderText("dupa", 0, 0, 1.0f, false);

	if (renderDepthMap == 1 || renderDepthMap == 2) {
		depthDebugShader->use();
		depthDebugShader->setInt("perspective", renderDepthMap == 1 ? 0 : 1);
		depthDebugShader->setFloat("near_plane", renderDepthMap == 1 ? dirNear : spotNear);
		depthDebugShader->setFloat("far_plane", renderDepthMap == 1 ? dirNear : spotFar);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderDepthMap == 1 ? dirLightShadows[0]->texture : spotLightShadows[0]->texture);

		static unsigned int quadVAO = 0;
		static unsigned int quadVBO;
		if (quadVAO == 0) {
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glUseProgram(0);
	}
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
			camera->rotateX(mouseMovementX * deltaTime);
		}
		if (abs(mouseMovementY) < 1000.0f) {
			camera->rotateY(-mouseMovementY * deltaTime);
		}
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

void TestScene::renderDirLights() {
	for (int i = 0; i < dirLights.size(); i++) {
		LightShadowData *data = dirLightShadows[i];
		DirLight *light = dirLights[i];
		DirLightNode *node = dirLightNodes[i];

		glViewport(0, 0, data->width, data->height);
		glBindFramebuffer(GL_FRAMEBUFFER, data->fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		depthShader->use();
		glm::vec3 position = glm::vec3(node->worldTransform.Matrix()[3]);
		glm::mat4 projection = glm::ortho(-dirProjSize, dirProjSize, -dirProjSize, dirProjSize, dirNear, dirFar);
		glm::mat4 directionWorld = node->worldTransform.Matrix();
		directionWorld[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		light->lightSpace = projection * lookAt(position, position + glm::vec3(directionWorld * -glm::vec4(glm::vec3(light->direction), 0.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
		depthShader->setLightSpace(light->lightSpace);
		rootNode->draw(depthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, sceneManager->getFramebuffer());
	}
}

void TestScene::renderSpotLights() {
	spotLightProjection = glm::perspective(glm::radians(45.0f), 1.0f, spotNear, spotFar);
	for (int i = 0; i < spotLights.size(); i++) {
		LightShadowData *data = spotLightShadows[i];
		SpotLight *light = spotLights[i];
		SpotLightNode *node = spotLightNodes[i];

		glViewport(0, 0, data->width, data->height);
		glBindFramebuffer(GL_FRAMEBUFFER, data->fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		depthShader->use();
		//glm::mat4 world = node->worldTransform.Matrix() * translate(glm::mat4(1.0f), glm::vec3(light->position));
		glm::mat4 world = node->worldTransform.Matrix();
		glm::vec3 pos = world[3];
		glm::mat4 directionWorld = world;
		directionWorld[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		light->lightSpace = spotLightProjection * lookAt(pos, pos + glm::vec3(directionWorld * glm::vec4(glm::vec3(light->direction), 0.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
		depthShader->setLightSpace(light->lightSpace);
		rootNode->draw(depthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, sceneManager->getFramebuffer());
	}
}

void TestScene::renderPointLights() {
	for (int i = 0; i < pointLights.size(); i++) {
		LightShadowData *data = pointLightShadows[i];
		PointLight *light = pointLights[i];
		PointLightNode *node = pointLightNodes[i];

		glViewport(0, 0, data->width, data->height);
		glBindFramebuffer(GL_FRAMEBUFFER, data->fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		depthPointShader->setFloat("near_plane", light->near_plane);
		depthPointShader->setFloat("far_plane", light->far_plane);
		glm::mat4 world = node->worldTransform.Matrix();
		glm::vec3 position = world[3];
		depthPointShader->setPointPosition(position);
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, light->near_plane, light->far_plane);

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

		for (int i = 0; i < 6; i++) {
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
		case KEY_TOGGLE_MOUSE_LOCK:
			if (pressed) {
				lockMouse = !lockMouse;
				sceneManager->setCursorLocked(lockMouse);
				initMouse = true;
			}
			break;
		case KEY_QUIT:
			sceneManager->goToMenu(true);
			//glfwSetWindowShouldClose(sceneManager->getWindow(), true);
			break;
	}
}
