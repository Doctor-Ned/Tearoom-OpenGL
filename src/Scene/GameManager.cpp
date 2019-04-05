#include "GameManager.h"
#include "GLFW/glfw3.h"
#include "AssetManager.h"
#include "Scene/Scenes/MenuScene.h"

GameManager *GameManager::getInstance() {
	static GameManager* instance = nullptr;
	if (instance == nullptr) {
		instance = new GameManager();
	}
	return instance;
}

void GameManager::setCurrentScene(Scene * scene) {
	currentScene = scene;
	if (currentScene != nullptr) {
		currentScene->updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);
	}
}

void GameManager::setCurrentSceneAndDeletePrevious(Scene* scene) {
	Scene* old = currentScene;
	currentScene = scene;

	delete old;

}

float GameManager::getWindowWidth() {
	return windowWidth;
}

float GameManager::getWindowHeight() {
	return windowHeight;
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

GameFramebuffers GameManager::getFramebuffers() {
	GameFramebuffers result;
	result.main = mainFramebuffer;
	result.ping = pingPongFramebuffers[0];
	result.pong = pingPongFramebuffers[1];
	result.ui = uiFramebuffer;
	return result;
}

void GameManager::setWindow(GLFWwindow* window) {
	this->window = window;
}

void GameManager::setCursorLocked(bool locked) {
	glfwSetInputMode(window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void GameManager::goToMenu(bool destroyPreviousScene) {
	Scene* old = currentScene;
	currentScene = menuScene;
	setCursorLocked(false);
	if (old != menuScene && destroyPreviousScene && reinterpret_cast<int>(old) != 0xCDCDCDCD) {
		delete old;
	}
}

void GameManager::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	windowCenterX = windowWidth / 2.0f;
	windowCenterY = windowHeight / 2.0f;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	if (currentScene != nullptr) {
		currentScene->updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);
	}
}

void GameManager::setup() {
	mainFramebuffer = createMultitextureFramebuffer(GL_RGB16F, windowWidth, windowHeight, GL_RGB, GL_FLOAT, 2);
	uiFramebuffer = createFramebuffer(GL_RGBA, windowWidth, windowHeight, GL_RGBA, GL_UNSIGNED_BYTE);
	pingPongFramebuffers[0] = createFramebuffer(GL_RGB16F, windowWidth, windowHeight, GL_RGB, GL_FLOAT);
	pingPongFramebuffers[1] = createFramebuffer(GL_RGB16F, windowWidth, windowHeight, GL_RGB, GL_FLOAT);
	renderbuffer = createDepthRenderbuffer(windowWidth, windowHeight);
	
	AssetManager::getInstance()->setup();
	menuScene = new MenuScene();
	goToMenu();
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
	glDeleteBuffers(1, &pingPongFramebuffers[0].texture);
	glDeleteFramebuffers(1, &pingPongFramebuffers[0].fbo);
	glDeleteBuffers(1, &pingPongFramebuffers[1].texture);
	glDeleteFramebuffers(1, &pingPongFramebuffers[1].fbo);

}

GLuint GameManager::createDepthRenderbuffer(GLsizei width, GLsizei height) {
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	return rbo;
}

Framebuffer GameManager::createFramebuffer(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type) {
	int oldFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);
	Framebuffer result;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &result.texture);
	glBindTexture(GL_TEXTURE_2D, result.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

void GameManager::render() {
	if (currentScene != nullptr) {
		currentScene->render();
	}
}

void GameManager::renderUi() {
	if(currentScene != nullptr) {
		currentScene->renderUi();
	}
}

void GameManager::update(double timeDelta) {
	if (currentScene != nullptr) {
		currentScene->update(timeDelta);
	}
}

void GameManager::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (currentScene != nullptr) {
		currentScene->keyboard_callback(window, key, scancode, action, mods);
	}
}

void GameManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (currentScene != nullptr) {
		currentScene->mouse_callback(window, xpos, ypos);
	}
}

void GameManager::mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
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