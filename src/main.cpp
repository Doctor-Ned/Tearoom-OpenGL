#include "Global.h"
#include <cstdio>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <GLFW/glfw3.h>
#include "freetype/ftparams.h"
#include <ctime>
#include "Scene/GameManager.h"
#include "Ui/UiElement.h"
#include "Render/Shader.h"
#include "Scene/Scenes/MiszukScene.h"
#include "Render/PostProcessingShader.h"
#include "Ui/UiPlane.h"
#include "Render/LightManager.h"
#include "Ui/UiText.h"
#include "Ui/UiColorPlane.h"
#include "Serialization/Serializer.h"
#include "Scene/Node.h"
#include "Scene/SoundSystem.h"

//comment extern below if you don't have NVidia GPU
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static GameManager* gameManager;
static AssetManager* assetManager;
static Serializer* serializer;

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	gameManager->keyboard_callback(window, key, scancode, action, mods);
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void char_callback(GLFWwindow *window, unsigned int c) {
	ImGui_ImplGlfw_CharCallback(window, c);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	gameManager->mouse_callback(window, xpos, ypos);
}

void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	gameManager->mouse_button_callback(window, butt, action, mods);
	ImGui_ImplGlfw_MouseButtonCallback(window, butt, action, mods);
}

int main(int argc, char** argv) {
	gameManager = GameManager::getInstance();
	assetManager = AssetManager::getInstance();
	serializer = Serializer::getInstance();

	bool fullscreen_borderless = false;
	bool borderless = false;
	bool fullscreen = false;
	bool windowSizeDefined = false;

	float windowWidth = 1280.0f, windowHeight = 720.0f;

	bool expectedWidth = false, expectedHeight = false;

	for (int i = 0; i < argc; i++) {
		char* arg = argv[i];

		if (expectedWidth || expectedHeight) {
			int target = 0;
			bool correct = true;
			int count = 0;
			for (count = 0; arg[count] != '\0'; count++) {
				if (arg[count] < '0' || arg[count] > '9') {
					correct = false;
					break;
				}
			}
			if (correct) {
				int multiplier = 1;
				for (int j = 0; j < count; j++) {
					target += multiplier * (arg[count - 1 - j] - '0');
					multiplier *= 10;
				}
				if (expectedWidth) {
					windowWidth = target;
					windowSizeDefined = true;
					expectedWidth = false;
				} else {
					windowHeight = target;
					windowSizeDefined = true;
					expectedHeight = false;
				}
			}
		} else {
			if (strcmp("-fullscreen", arg) == 0) {
				fullscreen = true;
			} else if (strcmp("-windowed", arg) == 0) {
				fullscreen = false;
			} else if (strcmp("-borderless", arg) == 0) {
				borderless = true;
			} else if (strcmp("-width", arg) == 0) {
				expectedWidth = true;
			} else if (strcmp("-height", arg) == 0) {
				expectedHeight = true;
			} else if (strcmp("-fs_borderless", arg) == 0) {
				fullscreen_borderless = true;
			}
		}
	}

	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;


	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 4.3 + GLSL 430
	const char* glsl_version = "#version 430";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

	float screenWidth = windowWidth, screenHeight = windowHeight;

	if (fullscreen || fullscreen_borderless) {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *mode = glfwGetVideoMode(monitor);
		screenWidth = mode->width;
		screenHeight = mode->height;
	}

	if (fullscreen && !windowSizeDefined || fullscreen_borderless) {
		windowWidth = screenWidth;
		windowHeight = screenHeight;
	}

	gameManager->updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight, false);

	// Create window with graphics context
	GLFWwindow* window;
	if (fullscreen_borderless) {
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		window = glfwCreateWindow(screenWidth, screenHeight, "Tearoom", nullptr, nullptr);
	} else if (borderless) {
		if (fullscreen) {
			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			window = glfwCreateWindow(screenWidth, screenHeight, "Tearoom", monitor, nullptr);
		} else {
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			window = glfwCreateWindow(screenWidth, screenHeight, "Tearoom", nullptr, nullptr);
		}
	} else if (fullscreen) {
		window = glfwCreateWindow(screenWidth, screenHeight, "Tearoom", glfwGetPrimaryMonitor(), nullptr);
	} else {
		window = glfwCreateWindow(screenWidth, screenHeight, "Tearoom", nullptr, nullptr);
	}
	if (window == nullptr) {
		return 1;
	}
	glfwMakeContextCurrent(window);
	//glfwSwapInterval(0); // vsync, now set by the GameManager

	gameManager->setWindow(window);

	// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = !gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
#endif
	if (err) {
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, false);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::StyleColorsDark();

	srand(time(nullptr));

	glfwMakeContextCurrent(window);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.01f);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, keyboard_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetCharCallback(window, char_callback);

	GLuint vao, vbo;

	UiTextureVertex vertices[4];
	vertices[0].TexCoords = glm::vec2(0.0f, 1.0f);
	vertices[1].TexCoords = glm::vec2(0.0f, 0.0f);
	vertices[2].TexCoords = glm::vec2(1.0f, 0.0f);
	vertices[3].TexCoords = glm::vec2(1.0f, 1.0f);

	vertices[0].Position = glm::vec2(-1.0f, 1.0f);
	vertices[1].Position = glm::vec2(-1.0f, -1.0f);
	vertices[2].Position = glm::vec2(1.0f, -1.0f);
	vertices[3].Position = glm::vec2(1.0f, 1.0f);

	std::vector<UiTextureVertex> data;
	data.push_back(vertices[0]);
	data.push_back(vertices[1]);
	data.push_back(vertices[2]);
	data.push_back(vertices[0]);
	data.push_back(vertices[2]);
	data.push_back(vertices[3]);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(UiTextureVertex), &data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UiTextureVertex), static_cast<void*>(nullptr));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UiTextureVertex),
		reinterpret_cast<void*>(offsetof(UiTextureVertex, TexCoords)));

	data.clear();
	glBindVertexArray(0);

	const glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);

	serializer->setup();
	gameManager->setup();
	SoundSystem::loadSounds();

	gameManager->updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);

	PostProcessingShader *postProcessingShader = dynamic_cast<PostProcessingShader*>(assetManager->getShader(STPostProcessing));
	postProcessingShader->use();
	postProcessingShader->setInt("scene", 0);
	postProcessingShader->setInt("bloomBlur", 1);
	postProcessingShader->setInt("ui", 2);
	postProcessingShader->setWindowSize(windowWidth, windowHeight);

	Shader *blurShader = assetManager->getShader(STBlur);

	GameFramebuffers framebuffers = gameManager->getFramebuffers();


	UiColorPlane *fpsPlane = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.9f), glm::vec2(0.0f, 0.0f), glm::vec2(120.0f, 30.0f), TopLeft);
	glm::vec2 planeCenter = fpsPlane->getPosition();
	planeCenter.x += fpsPlane->getSize().x / 2.0f;
	planeCenter.y += fpsPlane->getSize().y / 2.0f;
	UiText *fpsText = new UiText(planeCenter, fpsPlane->getSize(), "FPS: -", glm::vec3(1.0f, 1.0f, 1.0f), None);
	fpsPlane->addChild(fpsText);

	fpsPlane->updateDrawData();

	Shader* fpsPlaneShader = assetManager->getShader(fpsPlane->getShaderType()), *fpsTextShader = assetManager->getShader(fpsText->getShaderType());

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// Rendering
		static double currentTime, lastTime = 0.0, timeDelta, deltaSum = 100.0f;
		static int lastFps = 0;
		currentTime = glfwGetTime();
		timeDelta = currentTime - lastTime;
		lastTime = currentTime;
		lastFps = static_cast<int>(1.0 / timeDelta);
		deltaSum += timeDelta;
		if (deltaSum >= 0.5f) {   //update FPS text every second
			fpsText->setText("FPS: "+std::to_string(lastFps));
			deltaSum = 0.0f;
		}
		timeDelta <= 0.5 ? timeDelta : timeDelta = 1.0 / 120; //for debugging game loop
		gameManager->update(timeDelta);

		glEnable(GL_DEPTH_TEST);

		// Render to a separate framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.main.fbo);
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gameManager->render();

		bool horizontal = true, first_iteration = true;
		if (postProcessingShader->isBloomEnabled()) {
		// apply two-pass gaussian blur to bright fragments
			static const unsigned int amount = 10;
			blurShader->use();
			for (unsigned int i = 0; i < amount; i++) {
				if (!horizontal) {
					glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.ping.fbo);
				} else {
					glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.pong.fbo);
				}
				blurShader->setBool("horizontal", horizontal);
				glBindTexture(GL_TEXTURE_2D, first_iteration ? framebuffers.main.textures[1] : (horizontal ? framebuffers.ping.texture : framebuffers.pong.texture));
				glBindVertexArray(vao);
				glBindVertexBuffer(0, vbo, 0, sizeof(UiTextureVertex));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(0);
				horizontal = !horizontal;
				if (first_iteration) {
					first_iteration = false;
				}
			}
		}

		glDisable(GL_DEPTH_TEST);

		// Render UI to its framebuffer

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.ui.fbo);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glViewport(0, 0, windowWidth, windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gameManager->renderUi();
		fpsPlaneShader->use();
		fpsPlane->render(fpsPlaneShader);
		fpsTextShader->use();
		fpsText->render(fpsTextShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Render to the default framebuffer (screen) with post-processing
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		postProcessingShader->use();
		ImGui::Render();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebuffers.main.textures[0]);
		glActiveTexture(GL_TEXTURE1);
		if (horizontal) {
			glBindTexture(GL_TEXTURE_2D, framebuffers.ping.texture);
		} else {
			glBindTexture(GL_TEXTURE_2D, framebuffers.pong.texture);
		}
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, framebuffers.ui.texture);
		glBindVertexArray(vao);
		glBindVertexBuffer(0, vbo, 0, sizeof(UiTextureVertex));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

		bool loading = !assetManager->isLoaded();
		if (loading) {
			assetManager->loadNextPendingResource();
		}
		if(loading && assetManager->isLoaded()) {
			gameManager->goToMenu();
		}
	}
	delete gameManager;
	delete assetManager;
	delete LightManager::getInstance();
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
