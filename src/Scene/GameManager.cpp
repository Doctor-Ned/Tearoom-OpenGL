#include "GameManager.h"
#include "GLFW/glfw3.h"
#include "AssetManager.h"
#include "Scene/Scenes/MenuScene.h"
#include "Render/LightManager.h"
#include "Scenes/LoadingScene.h"
#include "Scenes/EditorScene.h"
#include "Profiler.h"

GameManager *GameManager::getInstance() {
	static GameManager* instance = nullptr;
	if (instance == nullptr) {
		instance = new GameManager();
	}
	return instance;
}

void GameManager::setCurrentScene(Scene * scene) {
	/*
	if (currentScene != scene) {
		keyCallbacks.clear();
		mouseCallbacks.clear();
	} */
	currentScene = scene;
	if (currentScene != nullptr) {
		currentScene->updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight, fov);
		currentScene->mouse_callback(window, mousePosition.x, mousePosition.y);
		LightManager::getInstance()->replaceLights(currentScene->getLights());
	}
}

void GameManager::setCurrentSceneAndDeletePrevious(Scene* scene) {
	Scene* old = currentScene;
	setCurrentScene(scene);
	delete old;
}

float GameManager::getWindowWidth() {
	return windowWidth;
}

float GameManager::getWindowHeight() {
	return windowHeight;
}

float GameManager::getFOV() {
	return fov;
}

float GameManager::getScreenWidth() {
	return screenWidth;
}

float GameManager::getScreenHeight() {
	return screenHeight;
}

float GameManager::getWindowCenterX() {
	return windowCenterX;
}

float GameManager::getWindowCenterY() {
	return windowCenterY;
}

GLuint GameManager::getMainFramebuffer() {
	return mainFramebuffer.fbo;
}

EditorScene* GameManager::getEditorScene() {
	return dynamic_cast<EditorScene*>(currentScene);
}

GameFramebuffers GameManager::getFramebuffers() {
	GameFramebuffers result;
	result.main = mainFramebuffer;
	for (int i = 0; i < BLOOM_TEXTURES; i++) {
		result.bloom[i] = bloomFramebuffers[i];
	}
	result.ui = uiFramebuffer;
	return result;
}

void GameManager::setWindow(GLFWwindow* window) {
	this->window = window;
}

void GameManager::setCursorLocked(bool locked) {
	glfwSetInputMode(window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	//if (!locked && cursorLocked) {
	//	glfwSetCursorPos(window, screenWidth / 2.0f, screenHeight / 2.0f);
	//}
	cursorLocked = locked;
}

bool GameManager::getCursorLocked() {
	return cursorLocked;
}

void GameManager::updateRescaleVectors() {
	uiRescaleVector = glm::vec2(UI_REF_WIDTH / screenWidth, UI_REF_HEIGHT / screenHeight);
	screenRescaleVector = glm::vec2(screenWidth / UI_REF_WIDTH, screenHeight / UI_REF_HEIGHT);
}

void GameManager::goToMenu(bool destroyPreviousScene) {
	Scene* old = currentScene;
	if (menuScene == nullptr) {
		menuScene = new MenuScene();
	}
	currentScene = menuScene;
	setCursorLocked(false);
	if (old != menuScene && destroyPreviousScene && reinterpret_cast<int>(old) != 0xCDCDCDCD) {
		delete old;
	}
}

void GameManager::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight, bool updateUiProjection) {
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	windowCenterX = windowWidth / 2.0f;
	windowCenterY = windowHeight / 2.0f;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	updateRescaleVectors();
	if (updateUiProjection) {
		UiElement::updateProjection();
	}
}

void GameManager::setup() {
	setVsync(enableVsync);

	AssetManager::getInstance()->setup();
	LightManager::getInstance()->setup();

	for (int i = 0; i < BLOOM_TEXTURES; i++) {
		GLuint height, width = height = round(pow(2.0f, 7.0f - i));
		//static const int basePow = 0;
		//GLuint width = round(glm::max(1.0f, windowWidth * pow(0.5f, basePow + i)));
		//GLuint height = round(glm::max(1.0f, windowHeight * pow(0.5f, basePow + i)));
		bloomFramebuffers[i].width = width;
		bloomFramebuffers[i].height = height;
		bloomFramebuffers[i].rescaler = createFramebuffer(GL_RGB16F, width, height, GL_RGB, GL_FLOAT);
		bloomFramebuffers[i].horizontal = createFramebuffer(GL_RGB16F, width, height, GL_RGB, GL_FLOAT);
		bloomFramebuffers[i].output = createFramebuffer(GL_RGB16F, width, height, GL_RGB, GL_FLOAT);
	}
	mainFramebuffer = createMultitextureFramebuffer(GL_RGB16F, windowWidth, windowHeight, GL_RGB, GL_FLOAT, 2);
	glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer.fbo);
	renderbuffer = createDepthRenderbuffer(windowWidth, windowHeight);
	uiFramebuffer = createFramebuffer(GL_RGBA, windowWidth, windowHeight, GL_RGBA, GL_UNSIGNED_BYTE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//menuScene = new MenuScene();
	//goToMenu();
	setCurrentScene(new LoadingScene());
}

Camera* GameManager::getCamera() const {
	if (currentScene != nullptr) {
		return currentScene->getCamera();
	}
	return nullptr;
}

bool GameManager::isVsyncEnabled() const {
	return enableVsync;
}

void GameManager::setVsync(bool enabled) {
	if (enabled != enableVsync) {
		glfwSwapInterval(enabled ? 1 : 0);
		enableVsync = enabled;
	}
}

void GameManager::addKeyCallback(int key, bool pressed, const std::function<void()>& callback) {
	for (auto &pair : keyCallbacks) {
		if (pair.first == key) {
			for (auto &pair2 : pair.second) {
				if (pair2.first == pressed) {
					pair2.second.push_back(callback);
					return;
				}
			}
			std::vector<std::function<void()>> vec;
			vec.push_back(callback);
			pair.second.emplace(pressed, vec);
			return;
		}
	}
	std::map<bool, std::vector<std::function<void()>>> map;
	std::vector<std::function<void()>> vec;
	vec.push_back(callback);
	map.emplace(pressed, vec);
	keyCallbacks.emplace(key, map);
}

void GameManager::addMouseCallback(int key, bool pressed, const std::function<void()>& callback) {
	for (auto &pair : mouseCallbacks) {
		if (pair.first == key) {
			for (auto &pair2 : pair.second) {
				if (pair2.first == pressed) {
					pair2.second.push_back(callback);
					return;
				}
			}
			std::vector<std::function<void()>> vec;
			vec.push_back(callback);
			pair.second.emplace(pressed, vec);
			return;
		}
	}
	std::map<bool, std::vector<std::function<void()>>> map;
	std::vector<std::function<void()>> vec;
	vec.push_back(callback);
	map.emplace(pressed, vec);
	mouseCallbacks.emplace(key, map);
}

GameManager::~GameManager() {
	if (currentScene == menuScene) {
		setCurrentScene(nullptr);
	}

	delete menuScene;
	delete currentScene;

	glDeleteBuffers(mainFramebuffer.textureAmount, reinterpret_cast<unsigned int *>(&mainFramebuffer.textures));
	glDeleteFramebuffers(1, &mainFramebuffer.fbo);
	glDeleteBuffers(1, &uiFramebuffer.texture);
	glDeleteFramebuffers(1, &uiFramebuffer.fbo);
	for (int i = 0; i < BLOOM_TEXTURES; i++) {
		glDeleteBuffers(1, &bloomFramebuffers[i].rescaler.texture);
		glDeleteFramebuffers(1, &bloomFramebuffers[i].rescaler.fbo);
		glDeleteBuffers(1, &bloomFramebuffers[i].horizontal.texture);
		glDeleteFramebuffers(1, &bloomFramebuffers[i].horizontal.fbo);
		glDeleteBuffers(1, &bloomFramebuffers[i].output.texture);
		glDeleteFramebuffers(1, &bloomFramebuffers[i].output.fbo);
	}
}

GLuint GameManager::createDepthRenderbuffer(GLsizei width, GLsizei height) {
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	return rbo;
}

Framebuffer GameManager::createFramebuffer(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, bool clamp, GLenum clampMode, glm::vec4 border, GLenum filter) {
	int oldFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);
	Framebuffer result;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &result.texture);
	glBindTexture(GL_TEXTURE_2D, result.texture);
	if (filter != GL_NONE) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	}
	if (clamp) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampMode);
		if (clampMode == GL_CLAMP_TO_BORDER) {
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, reinterpret_cast<float*>(&border));
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	glGenFramebuffers(1, &result.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, result.fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result.texture, 0);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	GLenum status;
	if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "glCheckFramebufferStatus: error %u", status);
		exit(6);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);
	return result;
}

Framebuffer GameManager::createFilteredFramebuffer(GLint internalFormat, GLsizei width, GLsizei height, GLenum format,
	GLenum type, GLenum filter) {
	return createFramebuffer(internalFormat, width, height, format, type, true, GL_CLAMP_TO_EDGE, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), filter);
}

SpecialFramebuffer GameManager::createSpecialFramebuffer(GLenum textureTarget, GLfloat filter, GLint internalFormat,
														 GLsizei width, GLsizei height, GLenum format, bool clamp, GLenum attachment, GLenum clampMethod) {
	SpecialFramebuffer result;
	int oldFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);
	glGenTextures(1, &result.texture);
	glBindTexture(textureTarget, result.texture);
	glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, filter);
	if (clamp) {
		glTexParameterf(textureTarget, GL_TEXTURE_WRAP_S, clampMethod);
		glTexParameterf(textureTarget, GL_TEXTURE_WRAP_T, clampMethod);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	glTexImage2D(textureTarget, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

	GLenum drawBuffer = GL_NONE;

	bool hasDepth = false;
	if (attachment == GL_DEPTH_ATTACHMENT) {
		hasDepth = true;
	} else {
		drawBuffer = attachment;
	}

	glGenFramebuffers(1, &result.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, result.fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureTarget, result.texture, 0);

	if (!hasDepth) {
		result.rbo = createDepthRenderbuffer(width, height);
	}

	glDrawBuffers(1, &drawBuffer);
	GLenum status;
	if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "glCheckFramebufferStatus: error %u", status);
		exit(6);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);

	return result;
}

MultitextureFramebuffer GameManager::createMultitextureFramebuffer(GLint internalFormat, GLsizei width, GLsizei height,
																   GLenum format, GLenum type, int textureCount) {
	int oldFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);
	MultitextureFramebuffer result;
	result.textureAmount = textureCount;
	result.textures = new GLuint[textureCount];
	glGenFramebuffers(1, &result.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, result.fbo);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(textureCount, result.textures);
	GLenum *drawBuffers = new GLenum[textureCount];
	for (int i = 0; i < textureCount; i++) {
		glBindTexture(GL_TEXTURE_2D, result.textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, result.textures[i], 0);
		drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	glDrawBuffers(textureCount, drawBuffers);
	GLenum status;
	if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "glCheckFramebufferStatus: error %u", status);
		exit(6);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);
	return result;
}

bool GameManager::getKeyState(const int key) {
	auto pair = keyStates.find(key);
	if (pair != keyStates.end()) {
		return pair->second;
	}
	return false;
}

bool GameManager::getKeyOnce(int key) {
	auto pair = keyStates.find(key);
	if (pair != keyStates.end()) {
		if (!pair->second) {
			return false;
		}

		auto keyProcessed = keysProcessed.find(key);
		if (keyProcessed == keysProcessed.end()) {
			keysProcessed.emplace(key, true);
			//here is the place key is processed
			return true;
		} else {
			//keyState is true but key was not processed
			if (!keyProcessed->second) {
				keysProcessed[key] = true;
				return true;
			}
			//keyState is true but key was processed one frame earlier
			return false;
		}
	}
	return false;
}

void GameManager::setKeyState(int key, bool pressed) {
	auto pair = keyStates.find(key);
	if (pair != keyStates.end()) {
		pair->second = pressed;
	} else {
		keyStates.emplace(key, pressed);
	}
}

void GameManager::keyEvent(const int key, const bool pressed) const {
	for (auto &pair : keyCallbacks) {
		bool found = false;
		if (pair.first == key) {
			for (auto &pair2 : pair.second) {
				if (pair2.first == pressed) {
					for (auto &callback : pair2.second) {
						callback();
					}
					found = true;
					break;
				}
			}
		}
		if (found) {
			break;
		}
	}
	if (currentScene != nullptr) {
		currentScene->keyEvent(key, pressed);
	}
}

bool GameManager::getMouseState(const int key) {
	auto pair = mouseStates.find(key);
	if (pair != mouseStates.end()) {
		return pair->second;
	}

	return false;
}

glm::vec2 GameManager::getMousePosition() const {
	return mousePosition;
}

glm::vec2 GameManager::getUiRescaleVector() const {
	return uiRescaleVector;
}

glm::vec2 GameManager::getScreenRescaleVector() const {
	return screenRescaleVector;
}

Scene* GameManager::getCurrentScene() {
	return currentScene;
}

Scene* GameManager::getCurrentNonEditorScene() {
	if (currentScene == nullptr) {
		return nullptr;
	}
	EditorScene* sc = getEditorScene();
	if (sc == nullptr) {
		return currentScene;
	}
	return sc->editedScene;
}

Camera* GameManager::getCurrentCamera() {
	return currentScene == nullptr ? nullptr : currentScene->getCamera();
}

Camera* GameManager::getCurrentNonEditorCamera() {
	if (currentScene == nullptr) {
		return nullptr;
	}
	EditorScene *sc = getEditorScene();
	if (sc == nullptr) {
		return currentScene->getCamera();
	}
	return sc->playerCamera;
}

void GameManager::setMouseState(int key, bool pressed) {
	auto pair = mouseStates.find(key);
	if (pair != mouseStates.end()) {
		pair->second = pressed;
	} else {
		mouseStates.emplace(key, pressed);
	}
}

void GameManager::mouseEvent(int key, bool pressed) {
	for (auto &pair : mouseCallbacks) {
		bool found = false;
		if (pair.first == key) {
			for (auto &pair2 : pair.second) {
				if (pair2.first == pressed) {
					for (auto &callback : pair2.second) {
						callback();
					}
					found = true;
					break;
				}
			}
		}
		if (found) {
			break;
		}
	}
	if (currentScene != nullptr) {
		currentScene->mouseEvent(key, pressed);
	}
}

void GameManager::render() {
	if (currentScene != nullptr) {
		currentScene->render();
	}
}

void GameManager::renderUi() {
	if (currentScene != nullptr) {
		currentScene->renderUi();
		if (Profiler::getInstance()->getEnabled()) {
			Profiler::getInstance()->renderProfilerWindow();
		}
	}
}

void GameManager::update(double timeDelta) {
	if (currentScene != nullptr) {
		currentScene->update(timeDelta);
	}
}

void GameManager::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		if (getKeyState(key)) {
			setKeyState(key, false);
			keyEvent(key, false);
			keysProcessed[key] = false;
		}
	}
	if (action == GLFW_PRESS) {
		if (!getKeyState(key)) {
			setKeyState(key, true);
			keyEvent(key, true);
		}
	}
	if (currentScene != nullptr) {
		currentScene->keyboard_callback(window, key, scancode, action, mods);
	}
}

void GameManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	mousePosition.x = xpos;
	mousePosition.y = ypos;
	if (currentScene != nullptr) {
		currentScene->mouse_callback(window, xpos, ypos);
	}
}

void GameManager::mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	if (action == GLFW_RELEASE) {
		if (getMouseState(butt)) {
			setMouseState(butt, false);
			mouseEvent(butt, false);
		}
	}
	if (action == GLFW_PRESS) {
		if (!getMouseState(butt)) {
			setMouseState(butt, true);
			mouseEvent(butt, true);
		}
	}
	if (currentScene != nullptr) {
		currentScene->mouse_button_callback(window, butt, action, mods);
	}
}

GLFWwindow* GameManager::getWindow() {
	return window;
}

void GameManager::quit() {
	glfwSetWindowShouldClose(window, true);
}