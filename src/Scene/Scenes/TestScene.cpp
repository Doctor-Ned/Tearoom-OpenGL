#include "TestScene.h"
#include "Mesh/MeshColorPlane.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/Model.h"
#include "Scene/Components/LightComponents/DirLightComp.h"
#include "Scene/Components/LightComponents/SpotLightComp.h"
#include "Scene/Components/LightComponents/SpotLightComp.h"
#include "Scene/Components/RotatingObject.h"
#include "Render/LightManager.h"
#include "Mesh/MeshPlane.h"
#include "Scene/Components/Billboard.h"
#include <iostream>
#include "Scene/Scripts/CollisionTest.h"
#include "Scene/Components/LightComponents/PointLightComp.h"
#include "Serialization/Serializer.h"
#include "Scene/Components/Animation.h"
#include "Scene/Components/KeyFrameAnimation.h"
#include "Scene/Components/SphereCollider.h"
#include "Scene/Components/BoxCollider.h"
#include "Scene/Components/Camera.h"
#include "Scene/Scripts/PlayerMovement.h"
#include "Ui/UiColorPlane.h"

TestScene::TestScene() : TestScene(false) {}

void TestScene::render() {
	rootNode->updateDrawData();

	lightManager->renderAndUpdate([this](Shader *shader) {
		renderNodesUsingRenderMap(shader, true);
		renderNodesUsingTransparentRenderMap(shader, true);
	}, updatableShaders);

	pointLightSphere->setColor(lights.pointLights[0]->color);

	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto &shader : updatableShaders) {
		shader->use();
		shader->setViewPosition(camera->getGameObject()->worldTransform.getPosition());
	}
	uboViewProjection->inject(camera->getView(), projection);

	renderNodesUsingRenderMap();

	static Shader *skyboxShader = assetManager->getShader(STSkybox);
	skybox->draw(skyboxShader, camera->getUntranslatedView(), projection);

	renderNodesUsingTransparentRenderMap();
}

void TestScene::renderUi() {
	Scene::renderUi();

	rootNode->getChild(1)->drawGui();

	sun->renderGui();

	if (modelNode != nullptr) {
		static float opacity = 1.0f;
		ImGui::SliderFloat("Model opacity", &opacity, 0.0f, 1.0f);
		modelNode->setOpacity(opacity);
		modelNode->drawGui();
	}

	//dirLightNode->getParent()->drawGui();
	//dirLightNode->drawGui();
	spotLightComps[0]->drawGui();
	pointLightComps[0]->drawGui();

	ImGui::NewLine();
	if (ImGui::Button("SAVE TO FILE")) {
		Serializer::getInstance()->saveScene(this, "test");
	}
}

SerializableType TestScene::getSerializableType() {
	return STestScene;
}

Json::Value TestScene::serialize(Serializer* serializer) {
	Json::Value root = Scene::serialize(serializer);
	for (int i = 0; i < dirLightComps.size(); i++) {
		root["dirLightComps"][i] = serializer->serialize(dirLightComps[i]);
	}
	for (int i = 0; i < spotLightComps.size(); i++) {
		root["spotLightComps"][i] = serializer->serialize(spotLightComps[i]);
	}
	for (int i = 0; i < pointLightComps.size(); i++) {
		root["pointLightComps"][i] = serializer->serialize(pointLightComps[i]);
	}
	root["modelNode"] = serializer->serialize(modelNode);
	root["pointLightSphere"] = serializer->serialize(pointLightSphere);
	root["sunNode"] = serializer->serialize(sunNode);
	root["sun"] = serializer->serialize(sun);
	return root;
}

void TestScene::deserialize(Json::Value& root, Serializer* serializer) {
	Scene::deserialize(root, serializer);
	for (int i = 0; i < root["dirLightComps"].size(); i++) {
		dirLightComps.push_back(dynamic_cast<DirLightComp*>(serializer->deserialize(root["dirLightComps"][i]).object));
	}
	for (int i = 0; i < root["spotLightComps"].size(); i++) {
		spotLightComps.push_back(dynamic_cast<SpotLightComp*>(serializer->deserialize(root["spotLightComps"][i]).object));
	}
	for (int i = 0; i < root["pointLightComps"].size(); i++) {
		pointLightComps.push_back(dynamic_cast<PointLightComp*>(serializer->deserialize(root["pointLightComps"][i]).object));
	}
	modelNode = dynamic_cast<GraphNode*>(serializer->deserialize(root["modelNode"]).object);
	pointLightSphere = dynamic_cast<MeshColorSphere*>(serializer->deserialize(root["pointLightSphere"]).object);
	sunNode = dynamic_cast<GraphNode*>(serializer->deserialize(root["sunNode"]).object);
	sun = dynamic_cast<Sun*>(serializer->deserialize(root["sun"]).object);
}

TestScene::TestScene(bool serialized) {
	std::vector<std::string> faces;
	faces.emplace_back("res/skybox/test/right.jpg");
	faces.emplace_back("res/skybox/test/left.jpg");
	faces.emplace_back("res/skybox/test/top.jpg");
	faces.emplace_back("res/skybox/test/bottom.jpg");
	faces.emplace_back("res/skybox/test/front.jpg");
	faces.emplace_back("res/skybox/test/back.jpg");

	getUiRoot()->addChild(new UiColorPlane(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(UI_REF_WIDTH, UI_REF_HEIGHT), TopLeft));
	getUiRoot()->addChild(new UiColorPlane(glm::vec4(0.0f, 1.0f, 0.0f, 0.5f), glm::vec2(UI_REF_WIDTH/2.0f, UI_REF_HEIGHT/2.0f), glm::vec2(200.0f, 100.0f), Center));


	if (!serialized) {
		GraphNode *camNode = new GraphNode(nullptr, rootNode);
		camNode->localTransform.setPosition(0.0f, 1.0f, 0.0f);
		PlayerMovement *mov = new PlayerMovement(camNode);
		camNode->addComponent(mov);
		mov->setFly(true);
		mov->setGravity(false);
		camera = camNode->getComponent<Camera>();

		skybox = new Skybox(faces);
		Model *model = new Model("res/models/muro/muro.obj");
		modelNode = new GraphNode(model, rootNode);
		modelNode->localTransform.initialize(-5.0f, 0.0f, 0.0f, 0.01f);

		GraphNode *rock = new GraphNode(new Model("res/models/pbr_rock/sharprockfree.obj"), rootNode);
		rock->localTransform.setScale(0.01f);

		MeshColorBox *floor = new MeshColorBox(glm::vec3(10.0f, 1.0f, 10.0f), glm::vec4(0.1f, 1.0f, 0.1f, 1.0f));

		MeshColorSphere *sphere = new MeshColorSphere(0.125f, 30, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		GraphNode* floorNode = new GraphNode(floor, rootNode);
		floorNode->localTransform.translate(glm::vec3(0.0f, -0.5f, 0.0f));
		GraphNode* sphereNode = new GraphNode(sphere, rootNode);
		sphereNode->localTransform.translate(0.0f, 1.0f, 0.0f);

		MeshColorSphere *lightSphere = new MeshColorSphere(0.05f, 30, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		lightSphere->setUseLight(false);

		lights = lightManager->createUnmanagedLights(2, 1, 1);
		sunNode = new GraphNode(nullptr, rootNode);
		sun = new Sun(lights.dirLights[0], lights.dirLights[1],
			normalize(glm::vec4(254, 107, 0, 255)),
			normalize(glm::vec4(173, 161, 70, 255)),
			normalize(glm::vec4(0, 2, 15, 255)),
			normalize(glm::vec4(2, 5, 18, 255)), 10.0f, 12.0f, glm::radians(75.0f), sunNode);
		sunNode->addComponent(sun);

		MeshColorBox* box = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		MeshColorBox* box1 = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
		MeshColorBox* box2 = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		MeshColorBox* floorMesh = new MeshColorBox(glm::vec3(-10.0f, -0.5f, -10.5f), glm::vec3(10.0f, 0.5f, 10.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//Model* sphere2 = new Model("res/models/sphere/sphere.obj");
		MeshColorBox* meshBox = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		MeshColorBox* meshBox1 = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
		GraphNode* boxNode = new GraphNode(box, rootNode);
		GraphNode* boxNode2 = new GraphNode(box1, rootNode);
		//GraphNode* sphereNode2 = new GraphNode(sphere2, rootNode);
		GraphNode* simpleBox1 = new GraphNode(meshBox, rootNode);
		GraphNode* simpleBox2 = new GraphNode(meshBox1, rootNode);
		GraphNode* pivot = new GraphNode(nullptr, rootNode);
		GraphNode* planete = new GraphNode(box2, pivot);
		GraphNode* floor1 = new GraphNode(floorMesh, rootNode);

		boxNode->localTransform.translate(glm::vec3(4.0f, 3.0f, 2.5f));
		boxNode->localTransform.rotateYDegrees(130.0f);
		boxNode2->localTransform.setPosition(7.0f, 3.0f, 3.0f);
		//sphereNode2->localTransform.translate(glm::vec3(-2.0f, 0.0f, 0.0f));
		simpleBox1->localTransform.setPosition(0.0f, 2.0f, 0.0f);
		planete->localTransform.setPosition(7.0f, 3.0f, 0.0f);
		simpleBox2->localTransform.setPosition(0.0f, 0.0f, 1.0f);
		floor1->localTransform.setPosition(0.0f, -3.0f, 0.0f);

		boxNode2->addComponent(new SphereCollider(boxNode2, DYNAMIC, true, glm::vec3(-0.5f, 0.0f, 0.0f), 1.0f));

		boxNode->addComponent(new BoxCollider(boxNode, DYNAMIC, true, glm::vec3(1, 0, 0), glm::vec3(1.3f, 1.0f, 0.5f)));
		boxNode->addComponent(new CollisionTest(boxNode));
		simpleBox1->addComponent(new BoxCollider(simpleBox1, STATIC, false, glm::vec3(0, 0, 0), glm::vec3(0.5f, 1.0f, 0.5f)));
		simpleBox2->addComponent(new BoxCollider(simpleBox2, STATIC, false, glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.5f, 0.5f)));
		pivot->addComponent(new BoxCollider(pivot, DYNAMIC, true, glm::vec3(7.0f, 3.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.5f)));
		pivot->addComponent(new CollisionTest(pivot));

		floor1->addComponent(new BoxCollider(floor1, STATIC, false, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.5f, 10.0f)));

		//miszuk animation
		GraphNode* doorPivot = new GraphNode(nullptr, rootNode);
		MeshColorBox* doorMesh = new MeshColorBox(glm::vec3(2.0f, 3.0f, 0.2f), glm::vec4(0.8f, 1.0f, 1.0f, 1.0f));
		MeshColorBox* handleMesh = new MeshColorBox(glm::vec3(0.2f, 0.4f, 0.4f), glm::vec4(0.8f, 0.0f, 0.0f, 1.0f));
		doorPivot->setName("doorPivot");
		GraphNode* door = new GraphNode(doorMesh, doorPivot);
		door->setName("door");
		GraphNode* handle = new GraphNode(handleMesh, door);
		handle->setName("handle");

		door->addComponent(new BoxCollider(door, STATIC, false, glm::vec3(0), glm::vec3(1.0f, 1.5f, 0.1f)));
		Animation* anim = new KeyFrameAnimation(door, "doorOpening");
		anim->addKeyFrame("door", anim::TRANSLATION, 0.0f, glm::vec3(0));
		anim->addKeyFrame("door", anim::TRANSLATION, 1.0f, glm::vec3(0.4f, 0.0f, 0.0f));
		anim->addKeyFrame("door", anim::TRANSLATION, 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		door->addComponent(anim);
		doorPivot->localTransform.setPosition(-4.0f, 2.0f, -2.0f);
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

		anim->deleteKeyFrame("handle", anim::ROTATION, 2.0f);
		//


		GraphNode *rotatingNode2 = new GraphNode(nullptr, rootNode);
		rotatingNode2->addComponent(new RotatingObject(0.075f, rotatingNode2));

		SpotLight *spotLight = lights.spotLights[0];
		spotLight->color = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
		spotLight->constant = 0.18f;
		spotLight->linear = 0.1f;
		spotLight->quadratic = 0.1f;
		GraphNode *spotLightNode = new GraphNode(lightSphere, rotatingNode2);
		spotLightNode->addComponent(new SpotLightComp(spotLight, spotLightNode));
		spotLightNode->localTransform.translate(0.0f, 1.0f, 3.0f);

		GraphNode *rotatingNode3 = new GraphNode(nullptr, rootNode);
		rotatingNode3->addComponent(new RotatingObject(0.15f, rotatingNode3));

		PointLight *pointLight = lights.pointLights[0];
		pointLight->color = glm::vec4(0.3f, 0.3f, 0.1f, 1.0f);
		pointLight->constant = 0.18f;
		pointLight->linear = 0.1f;
		pointLight->quadratic = 0.1f;
		pointLightSphere = new MeshColorSphere(0.125f, 30, pointLight->color);
		pointLightSphere->setShaderType(STLight);
		GraphNode *pointLightNode = new GraphNode(pointLightSphere, rotatingNode3);
		pointLightNode->localTransform.translate(0.5f, 0.5f, 0.0f);
		pointLightNode->addComponent(new PointLightComp(pointLight, pointLightNode));

		spotLightComps.push_back(spotLightNode->getComponent<SpotLightComp>());
		pointLightComps.push_back(pointLightNode->getComponent<PointLightComp>());

		GraphNode *billboardNode = new GraphNode(nullptr, rootNode);
		Billboard *billboard = new Billboard(camera, billboardNode, true);
		billboardNode->localTransform.translate(3.0f, 1.0f, -3.0f);
		billboardNode->addComponent(billboard);
		MeshPlane *emote = new MeshPlane(0.25f, 0.25f, "res/textures/face.png");
		emote->setUseLight(false);
		emote->setOpaque(false);
		GraphNode *emoteNode = new GraphNode(emote, billboardNode);
		lightIgnoredObjects.push_back(emoteNode);
		emoteNode->localTransform.rotateXDegrees(90.0f);

		reinitializeRenderMap();
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
				setCursorLocked(!getCursorLocked());
			}
			break;
		case KEY_QUIT:
			gameManager->goToMenu();
			break;
	}
}
