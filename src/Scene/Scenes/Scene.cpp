#include "Scene.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"
#include "Render/LightManager.h"
#include "Mesh/Mesh.h"
#include "Serialization/Serializer.h"
#include "Render/Camera.h"
#include "Ui/UiCanvas.h"
#include "Mesh/MeshRef.h"

void Scene::render() {
	Camera *camera = getCamera();
	if (camera != nullptr) {
		rootNode->updateDrawData();
		lightManager->renderAndUpdate([this](Shader *shader) {
			renderNodesUsingRenderMap(shader, true);
			renderNodesUsingTransparentRenderMap(shader, true);
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
		for(auto &refNode : refNodes) {
			MeshRef *ref = dynamic_cast<MeshRef*>(refNode->getMesh());
			ref->regenEnvironmentMap(refNode->worldTransform.getMatrix(), [this,refNode](glm::mat4 view, glm::mat4 projection) {
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
			shader->setViewPosition(camera->getPos());
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

void Scene::renderUi() {
	rootUiElement->updateDrawData();
	renderUiUsingRenderMap();
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
	if (node->getParent() != nullptr) {
		node->getParent()->removeChild(node);
	}
	for (auto &comp : node->getComponents()) {
		Renderable*r = dynamic_cast<Renderable*>(comp);
		if (r) {
			removeFromRenderMap(r);
		}
	}
	removeFromRenderMap(node, recurse);
}

void Scene::renderNodesUsingRenderMap(Shader* shader, bool ignoreLight, bool frustumCulling, bool ignoreRefractive) {
	renderFromMap(true, shader, ignoreLight, frustumCulling, ignoreRefractive);
}

void Scene::renderNodesUsingTransparentRenderMap(Shader* shader, bool ignoreLight, bool frustumCulling, bool ignoreRefractive) {
	renderFromMap(false, shader, ignoreLight, frustumCulling, ignoreRefractive);
}

void Scene::renderUiUsingRenderMap(Shader* shader) {
	if (shader == nullptr) {
		for (auto &type : ShaderTypes) {
			if (type != STNone) {
				shader = shaders[type];
				if (shader != nullptr) {
					shader->use();
				}
				for (auto &uiElement : *uiRenderMap[type]) {
					uiElement->render(shader);
				}
			}
		}
	} else {
		shader->use();
		for (auto &type : ShaderTypes) {
			if (type != STNone) {
				for (auto &uiElement : *uiRenderMap[type]) {
					uiElement->render(shader);
				}
			}
		}
	}
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

void Scene::addToRenderMap(UiElement* uiElement, bool recurse) {
	addToRenderMap(uiElement, recurse, true);
}

void Scene::updateRenderable(Renderable* renderable, bool addIfNotFound) {
	bool found = false;
	for (auto &type : ShaderTypes) {
		if (type != STNone) {
			auto vec = *renderMap[type];
			for (auto i = vec.begin(); i != vec.end();) {
				if (*i == renderable) {
					found = true;
					break;
				}
				++i;
			}
		}
		if (found) {
			break;
		}
	}
	if (found || addIfNotFound && !found) {
		addToRenderMap(renderable, false);
	}
}

void Scene::updateElement(UiElement* uiElement, bool addIfNotFound) {
	bool found = false;
	for (auto &type : ShaderTypes) {
		if (type != STNone) {
			auto vec = *uiRenderMap[type];
			for (auto i = vec.begin(); i != vec.end();) {
				if (*i == uiElement) {
					found = true;
					break;
				}
				++i;
			}
		}
		if (found) {
			break;
		}
	}
	if (found || addIfNotFound && !found) {
		addToRenderMap(uiElement, false);
	}
}

void Scene::removeComponent(GraphNode* node, Component* component) {
	node->removeComponent(component);
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

void Scene::removeFromRenderMap(UiElement* uiElement, bool recurse) {
	std::vector<UiElement*>* vec = uiRenderMap[uiElement->getShaderType()];
	for (auto i = vec->begin(); i != vec->end();) {
		if (*i == uiElement) {
			if (recurse) {
				for (auto &child : uiElement->getChildren()) {
					removeFromRenderMap(child, recurse);
				}
			}
			vec->erase(i);
			break;
		}
		++i;
	}
}

UiColorPlane* Scene::getInventoryBackground() {
	return inventoryBackground;
}

UiText* Scene::getInventoryText() {
	return  inventoryText;
}

std::vector<UiColorPlane*>* Scene::getObjectRepresentations() {
	return &objectRepresentasions;
}

UiElement* Scene::getUiRoot() {
	return rootUiElement;
}

void Scene::reinitializeRenderMap() {
	for (auto &type : ShaderTypes) {
		if (type != STNone) {
			delete renderMap[type];
			renderMap.erase(type);
			renderMap.emplace(type, new std::vector<Renderable*>());
			delete uiRenderMap[type];
			uiRenderMap.erase(type);
			uiRenderMap.emplace(type, new std::vector<UiElement*>());
		}
	}
	addToRenderMap(rootNode, true, false);
	addToRenderMap(rootUiElement, true, false);
}

void Scene::update(double deltaTime) {
	Camera *camera = getCamera();
	if (getCursorLocked() && camera) {
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
	OctreeNode::getInstance()->CollisionTests();
	if (camera != nullptr) {
		camera->RecalculateFrustum();
		Frustum frustum = camera->getFrustum();
		OctreeNode::getInstance()->frustumCulling(frustum);
	}
	OctreeNode::getInstance()->CollisionTests();
}

void Scene::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	windowCenterX = windowWidth / 2.0f;
	windowCenterY = windowHeight / 2.0f;
	projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.1f, 100.0f);
}

void Scene::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {}

void Scene::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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

void Scene::mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	rootUiElement->mouse_button_callback(window, butt, action, mods);
}

Scene::Scene() {
	gameManager = GameManager::getInstance();
	assetManager = AssetManager::getInstance();
	lightManager = LightManager::getInstance();
	shaders = assetManager->getShaders();
	rootNode = new GraphNode();
	rootNode->setName("Root");
	rootUiElement = new UiCanvas(glm::vec2(0.0f, 0.0f), glm::vec2(windowWidth, windowHeight));
	updatableShaders.push_back(assetManager->getShader(STModel));
	updatableShaders.push_back(assetManager->getShader(STModelInstanced));
	updatableShaders.push_back(assetManager->getShader(STTexture));
	updatableShaders.push_back(assetManager->getShader(STColor));
	updatableShaders.push_back(assetManager->getShader(STReflect));
	updatableShaders.push_back(assetManager->getShader(STRefract));
	uboLights = assetManager->getUboLights();
	uboTextureColor = assetManager->getUboTextureColor();
	uboViewProjection = assetManager->getUboViewProjection();
	gameFramebuffers = gameManager->getFramebuffers();
	Scene::updateWindowSize(gameManager->getWindowWidth(), gameManager->getWindowHeight(), gameManager->getScreenWidth(), gameManager->getScreenHeight());
	for (auto &type : ShaderTypes) {
		if (type != STNone) {
			renderMap.emplace(type, new std::vector<Renderable*>());
			transparentRenderMap.emplace(type, new std::vector<Renderable*>());
			uiRenderMap.emplace(type, new std::vector<UiElement*>());
		}
	}
}

Scene::~Scene() {
	OctreeNode::toInsert2.clear();
	//delete rootUiElement;
	for (auto &type : ShaderTypes) {
		if (type != STNone) {
			delete renderMap[type];
			delete uiRenderMap[type];
		}
	}
	renderMap.clear();
	uiRenderMap.clear();
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

bool Scene::getKeyState(const int key) const {
	return gameManager->getKeyState(key);
}

bool Scene::getMouseState(const int key) const {
	return gameManager->getMouseState(key);
}

void Scene::keyEvent(int key, bool pressed) {}

void Scene::mouseEvent(int key, bool pressed) {}

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

void Scene::addToRenderMap(UiElement* uiElement, bool recurse, bool checkIfExists) {
	bool exists = false;
	if (uiElement->getShaderType() == STNone) {
		if (recurse) {
			for (auto &child : uiElement->getChildren()) {
				addToRenderMap(child, recurse, checkIfExists);
			}
		}
		return;
	}
	std::vector<UiElement*>* vec = uiRenderMap[uiElement->getShaderType()];
	if (checkIfExists) {
		for (auto &r : *vec) {
			if (r == uiElement) {
				exists = true;
				break;
			}
		}
	}
	if (!exists) {
		vec->push_back(uiElement);
		if (recurse) {
			for (auto &child : uiElement->getChildren()) {
				addToRenderMap(child, recurse, checkIfExists);
			}
		}
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
						for (auto i = octree->frustumContainer.begin(); i != octree->frustumContainer.end();) {
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
						for (auto i = octree->frustumContainer.begin(); i != octree->frustumContainer.end();) {
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