#include "Scene.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"
#include "Render/LightManager.h"
#include "Mesh/Mesh.h"
#include "Serialization/Serializer.h"
#include "Ui/UiCanvas.h"
#include "Mesh/MeshRef.h"
#include "Ui/UiPlane.h"
#include <iostream>
#include <Ui/UiButton.h>
#include "Scene/Components/LightComponents/DirLightComp.h"
#include "Scene/Components/LightComponents/SpotLightComp.h"
#include "Scene/Components/LightComponents/PointLightComp.h"
#include "Scene/Components/Camera.h"
#include "EditorScene.h"
#include "Profiler.h"
#include "Scene/SoundSystem.h"

void Scene::render() {
	Camera *camera = getCamera();
	if (camera != nullptr) {
		glm::vec3 pos = camera->getGameObject()->getPosition();
		sf::Listener::setPosition(pos.x, pos.y, pos.z);
		glm::vec3 front = camera->getGameObject()->getFrontVector();
		sf::Listener::setDirection(front.x, front.y, front.z);
		glm::vec3 up = camera->getGameObject()->getUpVector();
		sf::Listener::setUpVector(up.x, up.y, up.z);
		rootNode->updateDrawData();
		lightManager->renderAndUpdate([this](Shader *shader) {
			renderNodesUsingRenderMap(shader, true);
			//renderNodesUsingTransparentRenderMap(shader, true);
		}, updatableShaders);
		std::vector<GraphNode*> refNodes;
		for (auto &rend : *renderMap[STReflect]) {
			GraphNode *gn = dynamic_cast<GraphNode*>(rend);
			if (gn != nullptr) {
				MeshRef *ref = dynamic_cast<MeshRef*>(gn->getMesh());
				if (ref != nullptr) {
					refNodes.push_back(gn);
				}
			}
		}
		for (auto &rend : *renderMap[STRefract]) {
			GraphNode *gn = dynamic_cast<GraphNode*>(rend);
			if (gn != nullptr) {
				MeshRef *ref = dynamic_cast<MeshRef*>(gn->getMesh());
				if (ref != nullptr) {
					refNodes.push_back(gn);
				}
			}
		}
		glViewport(0, 0, ENVMAP_SIZE.x, ENVMAP_SIZE.y);
		for (auto &refNode : refNodes) {
			MeshRef *ref = dynamic_cast<MeshRef*>(refNode->getMesh());
			ref->regenEnvironmentMap(refNode->worldTransform.getMatrix(), [this, refNode](glm::mat4 view, glm::mat4 projection) {
				for (auto &shader : updatableShaders) {
					shader->use();
					shader->setViewPosition(refNode->worldTransform.getPosition());
				}
				uboViewProjection->inject(view, projection);
				renderNodesUsingRenderMap(nullptr, false, false, true);
				if (skybox != nullptr) {
					static Shader *skyboxShader = assetManager->getShader(STSkybox);
					glm::mat4 v = view;
					v[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
					skybox->draw(skyboxShader, v, projection);
				}
				renderNodesUsingTransparentRenderMap(nullptr, false, false, true);
			});
		}
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (auto &shader : updatableShaders) {
			shader->use();
			shader->setViewPosition(camera->getGameObject()->worldTransform.getPosition());
		}
		uboViewProjection->inject(camera->getView(), projection);
		renderNodesUsingRenderMap();
		if (skybox != nullptr) {
			static Shader *skyboxShader = assetManager->getShader(STSkybox);
			skybox->draw(skyboxShader, camera->getUntranslatedView(), projection);
		}
		renderNodesUsingTransparentRenderMap();
	}
}

void Scene::renderUiElement(UiElement* uiElement) {

	if (!uiElement->isActive()) {
		return;
	}
	Shader *shader = assetManager->getShader(uiElement->getShaderType());
	if (shader != nullptr) {
		shader->use();
		uiElement->render(shader);
	}
	for (auto &child : uiElement->getChildren()) {
		renderUiElement(child);
	}
}

void Scene::renderUi() {
	rootUiElement->updateDrawData();
	renderUiElement(rootUiElement);
	if (showPauseScene) {
		pauseScene->renderUi();
	}
}

Camera *Scene::getCamera() {
	return camera;
}

void Scene::setCamera(Camera* camera) {
	this->camera = camera;
}

void Scene::addRenderedNode(GraphNode* node, GraphNode* parent, bool recurse) {
	if (parent == nullptr) {
		parent = rootNode;
	}
	node->setParent(parent);
	addToRenderMap(node, recurse);
}

void Scene::removeNode(GraphNode* node, bool recurse) {
	if (recurse) {
		for (auto &nod : node->getChildren()) {
			removeNode(nod, true);
		}
	}
	if (node->getParent() != nullptr) {
		node->getParent()->removeChild(node);
	}
	for (auto &comp : node->getComponents()) {
		removeComponent(node, comp);
	}
	removeFromRenderMap(node, false);
}

void Scene::renderNodesUsingRenderMap(Shader* shader, bool ignoreLight, bool frustumCulling, bool ignoreRefractive) {
	renderFromMap(true, shader, ignoreLight, frustumCulling, ignoreRefractive);
}

void Scene::renderNodesUsingTransparentRenderMap(Shader* shader, bool ignoreLight, bool frustumCulling, bool ignoreRefractive) {
	renderFromMap(false, shader, ignoreLight, frustumCulling, ignoreRefractive);
}

void Scene::addComponent(GraphNode* node, Component* component) {
	node->addComponent(component);
	Renderable *r = dynamic_cast<Renderable*>(component);
	if (r) {
		addToRenderMap(r, false);
	}
}

void Scene::addToRenderMap(GraphNode* node, bool recurse) {
	addToRenderMap(node, recurse, true);
}

void Scene::addToRenderMap(Renderable* renderable) {
	addToRenderMap(renderable, true);
}

void Scene::updateRenderable(Renderable* renderable, bool addIfNotFound) {
	bool found = false;
	ShaderType foundType = STNone;
	for (auto &type : ShaderTypes) {
		if (type != STNone) {
			auto vec = *renderMap[type];
			for (auto i = vec.begin(); i != vec.end();) {
				if (*i == renderable) {
					found = true;
					foundType = type;
					break;
				}
				++i;
			}
		}
		if (found) {
			break;
		}
	}
	if (found || addIfNotFound) {
		if (found) {
			for (auto i = renderMap[foundType]->begin(); i != renderMap[foundType]->end();) {
				if (*i == renderable) {
					renderMap[foundType]->erase(i);
					break;
				}
				++i;
			}
		}
		addToRenderMap(renderable, false);
	}
	GraphNode *node = dynamic_cast<GraphNode*>(renderable);
	if (node != nullptr) {
		for (auto &rend : node->getRenderableComponents()) {
			updateRenderable(rend, addIfNotFound);
		}
	}
}

void Scene::removeComponent(GraphNode* node, Component* component) {
	node->removeComponent(component);
	{
		DirLightComp *dir = dynamic_cast<DirLightComp*>(component);
		if (dir) {
			LightManager::getInstance()->remove(dir->getLight());
			setLights(LightManager::getInstance()->getLights());
		}
		SpotLightComp *spot = dynamic_cast<SpotLightComp*>(component);
		if (spot) {
			LightManager::getInstance()->remove(spot->getLight());
			setLights(LightManager::getInstance()->getLights());
		}
		PointLightComp *point = dynamic_cast<PointLightComp*>(component);
		if (point) {
			LightManager::getInstance()->remove(point->getLight());
			setLights(LightManager::getInstance()->getLights());
		}
		Camera *cam = dynamic_cast<Camera*>(component);
		if (cam) {
			if (camera == cam) {
				camera = nullptr;
			}
			EditorScene *editor = gameManager->getEditorScene();
			if (editor && editor->playerCamera == cam) {
				editor->playerCamera = nullptr;
				editor->setEditorCamera(true);
			}
		}
	}
	Renderable *r = dynamic_cast<Renderable*>(component);
	if (r) {
		removeFromRenderMap(r);
	}
}

void Scene::removeFromRenderMap(GraphNode* node, bool recurse) {
	Renderable *r = dynamic_cast<Renderable*>(node);
	removeFromRenderMap(r);
	if (recurse) {
		for (auto &nod : node->getChildren()) {
			removeFromRenderMap(nod, true);
		}
	}
}

std::vector<UiPlane*>* Scene::getObjectRepresentations() {
	return &objectRepresentasions;
}

UiElement* Scene::getUiRoot() {
	return rootUiElement;
}

void Scene::setLights(Lights lights) {
	this->lights = lights;
}

void Scene::reinitializeRenderMap() {
	for (auto &type : ShaderTypes) {
		if (type != STNone) {
			delete renderMap[type];
			renderMap.erase(type);
			renderMap.emplace(type, new std::vector<Renderable*>());
		}
	}
	addToRenderMap(rootNode, true, false);
}

void Scene::update(double deltaTime) {
	if (showPauseScene) {
		pauseScene->update(deltaTime);
	} else {
		if (!rootNode->getChildren().empty()) {
			Profiler::getInstance()->startCountingTime();
			OctreeNode::getInstance().RebuildTree(50.0f);
			Profiler::getInstance()->addMeasure("Octree rebuild");

			Profiler::getInstance()->startCountingTime();
			OctreeNode::getInstance().Calculate();
			Profiler::getInstance()->addMeasure("Octree calculation");

			Profiler::getInstance()->startCountingTime();
			OctreeNode::getInstance().CollisionTests();
			Profiler::getInstance()->addMeasure("Collisions detection");

			if (camera != nullptr) {
				camera->RecalculateFrustum();
				Frustum frustum = camera->getFrustum();
				Profiler::getInstance()->startCountingTime();
				OctreeNode::getInstance().frustumCulling(frustum);
				Profiler::getInstance()->addMeasure("Frustum Culling");
			}
		}

		rootNode->update(deltaTime);
	}
}

void Scene::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight, float fov) {
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	windowCenterX = windowWidth / 2.0f;
	windowCenterY = windowHeight / 2.0f;
	projection = glm::perspective(glm::radians(fov), windowWidth / windowHeight, 0.1f, 100.0f);
}

void Scene::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (showPauseScene) {
		pauseScene->keyboard_callback(window, key, scancode, action, mods);
	}
}

void Scene::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (showPauseScene) {
		pauseScene->mouse_callback(window, xpos, ypos);
	} else {
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

		rootUiElement->mouse_callback(window, xpos, ypos);
	}
}

void Scene::mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	if (!showPauseScene) {
		rootUiElement->mouse_button_callback(window, butt, action, mods);
	} else {
		pauseScene->mouse_button_callback(window, butt, action, mods);
	}
}

Scene::Scene() {
	gameManager = GameManager::getInstance();
	assetManager = AssetManager::getInstance();
	lightManager = LightManager::getInstance();
	shaders = assetManager->getShaders();
	rootNode = new GraphNode();
	rootNode->setName("Root");
	rootUiElement = new UiCanvas(glm::vec2(0.0f, 0.0f), glm::vec2(UI_REF_WIDTH, UI_REF_HEIGHT));
	updatableShaders.push_back(assetManager->getShader(STModel));
	updatableShaders.push_back(assetManager->getShader(STAnimatedModel));
	updatableShaders.push_back(assetManager->getShader(STModelInstanced));
	updatableShaders.push_back(assetManager->getShader(STTexture));
	updatableShaders.push_back(assetManager->getShader(STColor));
	updatableShaders.push_back(assetManager->getShader(STReflect));
	updatableShaders.push_back(assetManager->getShader(STRefract));
	updatableShaders.push_back(assetManager->getShader(STText));
	uboLights = assetManager->getUboLights();
	uboTextureColor = assetManager->getUboTextureColor();
	uboViewProjection = assetManager->getUboViewProjection();
	gameFramebuffers = gameManager->getFramebuffers();
	Scene::updateWindowSize(gameManager->getWindowWidth(), gameManager->getWindowHeight(), gameManager->getScreenWidth(), gameManager->getScreenHeight(), gameManager->getFOV());
	for (auto &type : ShaderTypes) {
		if (type != STNone) {
			renderMap.emplace(type, new std::vector<Renderable*>());
			transparentRenderMap.emplace(type, new std::vector<Renderable*>());
		}
	}
}

Scene::~Scene() {
	OctreeNode::toInsert2.clear();
	//delete rootUiElement;
	for (auto &type : ShaderTypes) {
		if (type != STNone) {
			delete renderMap[type];
		}
	}
	renderMap.clear();
	delete rootNode;
}

Lights Scene::getLights() {
	return lights;
}

SerializableType Scene::getSerializableType() {
	return SScene;
}

Json::Value Scene::serialize(Serializer* serializer) {
	Json::Value root;
	root["rootNode"] = serializer->serialize(rootNode);
	for (int i = 0; i < lightIgnoredObjects.size(); i++) {
		root["lightIgnoredObjects"][i] = serializer->serialize(dynamic_cast<Serializable*>(lightIgnoredObjects[i]));
	}
	for (int i = 0; i < lights.dirLights.size(); i++) {
		root["lights"]["dirLights"][i] = serializer->serialize(lights.dirLights[i]);
	}
	for (int i = 0; i < lights.spotLights.size(); i++) {
		root["lights"]["spotLights"][i] = serializer->serialize(lights.spotLights[i]);
	}
	for (int i = 0; i < lights.pointLights.size(); i++) {
		root["lights"]["pointLights"][i] = serializer->serialize(lights.pointLights[i]);
	}
	root["camera"] = serializer->serialize(camera);
	root["skybox"] = serializer->serialize(skybox);
	return root;
}

void Scene::deserialize(Json::Value& root, Serializer* serializer) {
	rootNode = dynamic_cast<GraphNode*>(serializer->deserialize(root["rootNode"]).object);
	for (int i = 0; i < root["lightIgnoredObjects"].size(); i++) {
		lightIgnoredObjects.push_back(dynamic_cast<Renderable*>(serializer->deserialize(root["lightIgnoredObjects"][i]).object));
	}
	Json::Value jlights = root["lights"]["dirLights"];
	for (int i = 0; i < jlights.size(); i++) {
		lights.dirLights.push_back(dynamic_cast<DirLight*>(serializer->deserialize(jlights[i]).object));
	}
	jlights = root["lights"]["spotLights"];
	for (int i = 0; i < jlights.size(); i++) {
		lights.spotLights.push_back(dynamic_cast<SpotLight*>(serializer->deserialize(jlights[i]).object));
	}
	jlights = root["lights"]["pointLights"];
	for (int i = 0; i < jlights.size(); i++) {
		lights.pointLights.push_back(dynamic_cast<PointLight*>(serializer->deserialize(jlights[i]).object));
	}
	camera = dynamic_cast<Camera*>(serializer->deserialize(root["camera"]).object);
	skybox = dynamic_cast<Skybox*>(serializer->deserialize(root["skybox"]).object);
	reinitializeRenderMap();
}

GraphNode* Scene::getRootNode() const {
	return rootNode;
}

Skybox* Scene::getSkybox() {
	return skybox;
}

void Scene::setSkybox(Skybox* skybox) {
	this->skybox = skybox;
}

bool Scene::getCursorLocked() const {
	return gameManager->getCursorLocked();
}

void Scene::setCursorLocked(bool locked) const {
	gameManager->setCursorLocked(locked);
}

void Scene::setPauseScene(Scene* scene) {
	this->pauseScene = scene;
}

bool Scene::getKeyState(const int key) const {
	return gameManager->getKeyState(key);
}

bool Scene::getMouseState(const int key) const {
	return gameManager->getMouseState(key);
}

void Scene::keyEvent(int key, bool pressed) {
	if (pauseScene) {
		if (key == GLFW_KEY_ESCAPE && pressed) {
			showPauseScene = !showPauseScene;
			if (showPauseScene) {
				wasMouseLocked = getCursorLocked();
				setCursorLocked(false);
			} else {
				setCursorLocked(wasMouseLocked);
				initMouse = true;
			}
		}
	} else {
		if (showPauseScene) {
			pauseScene->keyEvent(key, pressed);
		} else {
			if (key == GLFW_KEY_F3 && pressed) {
				Profiler::getInstance()->setEnable(!Profiler::getInstance()->getEnabled());
			}
		}
	}
}

void Scene::mouseEvent(int key, bool pressed) {
	if (showPauseScene) {
		pauseScene->mouseEvent(key, pressed);
	}
}

void Scene::addToRenderMap(GraphNode* node, bool recurse, bool checkIfExists) {
	Renderable *r = dynamic_cast<Renderable*>(node);
	bool exists = false;
	std::vector<Renderable*>* vec = renderMap[r->getShaderType()];
	if (checkIfExists && r->getShaderType() != STNone) {
		for (auto &ren : *vec) {
			if (ren == r) {
				exists = true;
				break;
			}
		}
	}
	if (!exists) {
		if (r->getShaderType() != STNone) {
			vec->push_back(r);
		}
		for (auto &ren : node->getRenderableComponents()) {
			addToRenderMap(ren, checkIfExists);
		}
	}
	if (recurse) {
		for (auto &nod : node->getChildren()) {
			addToRenderMap(nod, recurse, checkIfExists);
		}
	}
}

void Scene::addToRenderMap(Renderable* renderable, bool checkIfExists) {
	if (renderable->getShaderType() == STNone) {
		return;
	}
	bool exists = false;
	std::vector<Renderable*>* vec = renderMap[renderable->getShaderType()];
	if (checkIfExists) {
		for (auto &r : *vec) {
			if (r == renderable) {
				exists = true;
				break;
			}
		}
	}
	if (!exists) {
		vec->push_back(renderable);
	}
}

void Scene::renderFromMap(bool opaque, Shader* shader, bool ignoreLight, bool frustumCulling, bool ignoreRefractive) {
	std::map<ShaderType, std::vector<Renderable*>*> *map = opaque ? &renderMap : &transparentRenderMap;
	auto octree = OctreeNode::getInstance();
	if (shader == nullptr) {
		for (auto &type : ShaderTypes) {
			if (type != STNone && !(ignoreLight && type == STLight)) {
				shader = shaders[type];
				shader->use();
				if (opaque) {
					transparentRenderMap[type]->clear();
				}
				for (auto &node : *(*map)[type]) {
					if (!node->isActive()) {
						continue;
					}
					if (ignoreLight) {
						bool ignore = false;
						for (auto &ignored : lightIgnoredObjects) {
							if (node == ignored) {
								ignore = true;
								break;
							}
						}
						if (ignore) {
							continue;
						}
					}
#ifdef ENABLE_FRUSTUM_CULLING
					if (!ignoreLight) {
						GraphNode* gn = dynamic_cast<GraphNode*>(node);
						if (gn == nullptr) {
							Component *comp = dynamic_cast<Component*>(node);
							gn = comp->getGameObject();
						}
						bool skip = true;
						for (auto i = octree.frustumContainer.begin(); i != octree.frustumContainer.end();) {
							if (*i == gn) {
								skip = false;
								break;
							}
							++i;
						}
						if (skip) {
							continue;
						}
					}
#endif
					if (opaque && !node->isOpaque()) {
						transparentRenderMap[type]->push_back(node);
						continue;
					}
					node->drawSelf(shader);
				}
			}
		}
	} else {
		shader->use();
		for (auto &type : ShaderTypes) {
			if (type != STNone && !(ignoreLight && type == STLight)) {
				if (opaque) {
					transparentRenderMap[type]->clear();
				}
				for (auto &node : *(*map)[type]) {
					if (!node->isActive()) {
						continue;
					}
					if (ignoreLight) {
						bool ignore = false;
						for (auto &ignored : lightIgnoredObjects) {
							if (node == ignored) {
								ignore = true;
								break;
							}
						}
						if (ignore) {
							continue;
						}
					}
#ifdef ENABLE_FRUSTUM_CULLING
					if (!ignoreLight) {
						GraphNode* gn = dynamic_cast<GraphNode*>(node);
						if (gn == nullptr) {
							Component *comp = dynamic_cast<Component*>(node);
							gn = comp->getGameObject();
						}
						bool skip = true;
						for (auto i = octree.frustumContainer.begin(); i != octree.frustumContainer.end();) {
							if (*i == gn) {
								skip = false;
								break;
							}
							++i;
						}
						if (skip) {
							continue;
						}
					}
#endif
					if (opaque && !node->isOpaque()) {
						transparentRenderMap[type]->push_back(node);
						continue;
					}
					node->drawSelf(shader);
				}
			}
		}
	}
}

void Scene::removeFromRenderMap(Renderable* renderable) {
	if (renderable->getShaderType() != STNone) {
		std::vector<Renderable*>* vec = renderMap[renderable->getShaderType()];
		for (auto i = vec->begin(); i != vec->end();) {
			if (*i == renderable) {
				vec->erase(i);
				break;
			}
			++i;
		}
	}
}