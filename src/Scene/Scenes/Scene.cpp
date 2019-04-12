#include "Scene.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"
#include "Render/LightManager.h"
#include "Mesh/Mesh.h"

void Scene::render() {
	rootNode->updateDrawData();
	renderNodesUsingRenderMap();
}

void Scene::renderUi() {
	for (auto &elem : uiElements) {
		elem->render();
	}
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
	if (shader == nullptr) {
		for (auto &type : ShaderTypes) {
			if (type != STNone && !(ignoreLight && type == STLight)) {
				shader = shaders[type];
				shader->use();
				for (auto &node : *renderMap[type]) {
					node->drawSelf(shader);
				}
			}
		}
	} else {
		shader->use();
		for (auto &type : ShaderTypes) {
			if (type != STNone && !(ignoreLight && type == STLight)) {
				for (auto &node : *renderMap[type]) {
					node->drawSelf(shader);
				}
			}
		}
	}
}

void Scene::addComponent(GraphNode* node, Component* component) {
	node->addComponent(component);
	Renderable *r = dynamic_cast<Renderable*>(component);
	if (r) {
		addToRenderMap(r);
	}
}

void Scene::addToRenderMap(Renderable* renderable) {
	if (renderable->getShaderType() != STNone) {
		renderMap[renderable->getShaderType()]->push_back(renderable);
	}
}

void Scene::addToRenderMap(GraphNode* node, bool recurse) {
	addToRenderMap(node, recurse, true);
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
	rootNode->update(deltaTime);
}

void Scene::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	windowCenterX = windowWidth / 2.0f;
	windowCenterY = windowHeight / 2.0f;
	for (auto &elem : uiElements) {
		elem->updateProjection(windowWidth, windowHeight, screenWidth, screenHeight);
	}
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
		}
	}
}

Scene::~Scene() {
	for (auto &elem : uiElements) {
		delete elem;
	}
	uiElements.clear();
	for (auto &type : ShaderTypes) {
		delete renderMap[type];
	}
	renderMap.clear();
	delete rootNode;
}

void Scene::addToRenderMap(GraphNode* node, bool recurse, bool checkIfExists) {
	Renderable *r = dynamic_cast<Renderable*>(node);
	if (r->getShaderType() != STNone) {
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
			vec->push_back(r);
			for (auto &ren : node->getRenderableComponents()) {
				addToRenderMap(ren, checkIfExists);
			}
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
	if (checkIfExists) {
		for (auto &r : *renderMap[renderable->getShaderType()]) {
			if (r == renderable) {
				exists = true;
				break;
			}
		}
	}
	if(!exists) {
		renderMap[renderable->getShaderType()]->push_back(renderable);
	}
}

void Scene::removeFromRenderMap(Renderable* renderable) {
	if (renderable->getShaderType() != STNone) {
		std::vector<Renderable*>* vec = renderMap[renderable->getShaderType()];
		for (auto i = vec->begin(); i != vec->end();) {
			if (*i == renderable) {
				vec->erase(i);
			}
			++i;
		}
	}
}
