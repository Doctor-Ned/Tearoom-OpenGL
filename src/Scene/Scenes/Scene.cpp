#include "Scene.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"
#include "Render/LightManager.h"
#include "Mesh/Mesh.h"
#include "Serialization/Serializer.h"

void Scene::render() {
	rootNode->updateDrawData();
	renderNodesUsingRenderMap();
}

void Scene::renderUi() {
	renderUiUsingRenderMap();
}

Camera *Scene::getCamera() {
	return nullptr;
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

void Scene::renderNodesUsingRenderMap(Shader* shader, bool ignoreLight) {
	renderFromMap(true, shader, ignoreLight);
}

void Scene::renderNodesUsingTransparentRenderMap(Shader* shader, bool ignoreLight) {
	renderFromMap(false, shader, ignoreLight);
}

void Scene::renderUiUsingRenderMap(Shader* shader) {
	if (shader == nullptr) {
		for (auto &type : ShaderTypes) {
			shader = shaders[type];
			if (shader != nullptr) {
				shader->use();
			}
			for (auto &uiElement : *uiRenderMap[type]) {
				uiElement->render(shader);
			}
		}
	} else {
		shader->use();
		for (auto &type : ShaderTypes) {
			for (auto &uiElement : *uiRenderMap[type]) {
				uiElement->render(shader);
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

void Scene::addToRenderMap(UiElement* uiElement) {
	addToRenderMap(uiElement, true);
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

void Scene::removeFromRenderMap(UiElement* uiElement) {
	std::vector<UiElement*>* vec = uiRenderMap[uiElement->getShaderType()];
	for (auto i = vec->begin(); i != vec->end();) {
		if (*i == uiElement) {
			vec->erase(i);
			break;
		}
		++i;
	}
}

void Scene::reinitializeRenderMap() {
	for (auto &type : ShaderTypes) {
		if (type != STNone) {
			delete renderMap[type];
			renderMap.erase(type);
			renderMap.emplace(type, new std::vector<Renderable*>());
		}
		delete uiRenderMap[type];
		uiRenderMap.erase(type);
		uiRenderMap.emplace(type, new std::vector<UiElement*>());
	}
	addToRenderMap(rootNode, true, false);
	for (auto &uiElement : uiElements) {
		addToRenderMap(uiElement, false);
	}
}

void Scene::update(double deltaTime) {
	rootNode->update(deltaTime);
}

void Scene::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	windowCenterX = windowWidth / 2.0f;
	windowCenterY = windowHeight / 2.0f;
}

void Scene::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {}

void Scene::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	for (auto &elem : uiElements) {
		elem->mouse_callback(window, xpos, ypos);
	}
}

void Scene::mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	for (auto &elem : uiElements) {
		elem->mouse_button_callback(window, butt, action, mods);
	}
}

Scene::Scene() {
	gameManager = GameManager::getInstance();
	assetManager = AssetManager::getInstance();
	lightManager = LightManager::getInstance();
	shaders = assetManager->getShaders();
	gameFramebuffers = gameManager->getFramebuffers();
	windowWidth = gameManager->getWindowWidth();
	windowHeight = gameManager->getWindowHeight();
	screenWidth = gameManager->getScreenWidth();
	screenHeight = gameManager->getScreenHeight();
	windowCenterX = windowWidth / 2.0f;
	windowCenterY = windowHeight / 2.0f;
	for (auto &type : ShaderTypes) {
		if (type != STNone) {
			renderMap.emplace(type, new std::vector<Renderable*>());
			transparentRenderMap.emplace(type, new std::vector<Renderable*>());
		}
		uiRenderMap.emplace(type, new std::vector<UiElement*>());
	}
}

Scene::~Scene() {
	for (auto &elem : uiElements) {
		delete elem;
	}
	OctreeNode::toInsert2.clear();
	uiElements.clear();
	for (auto &type : ShaderTypes) {
		if (type != STNone) {
			delete renderMap[type];
		}
		delete uiRenderMap[type];
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
	reinitializeRenderMap();
}

GraphNode* Scene::getRootNode() const {
	return rootNode;
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
	if (checkIfExists) {
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

void Scene::addToRenderMap(UiElement* uiElement, bool checkIfExists) {
	bool exists = false;
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
	}
}

void Scene::renderFromMap(bool opaque, Shader* shader, bool ignoreLight) {
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