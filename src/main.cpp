//#define ENABLE_GL_ERROR_CHECK
//#define ENABLE_GL_ERROR_BREAK
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
#include "Scene/Scenes/OptionsScene.h"
#include "Profiler.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <filesystem>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//comment extern below if you don't have NVidia GPU
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

static void glfw_error_callback(int error, const char* description) {
	SPDLOG_ERROR("GLFW error {}: {}", error, description);
}

static GameManager* gameManager;
static AssetManager* assetManager;
static Serializer* serializer;
static bool showUi = true;
static bool uiButtonPressed = false, ssPressed = false, uiSsPressed = false, fpsPressed = false, keysPressed = false;
static const int SHOW_UI_BUTTON = GLFW_KEY_F1, SS_BUTTON = GLFW_KEY_F5, SS_UI_BUTTON = GLFW_KEY_F6, FPS_BUTTON = GLFW_KEY_F2, SHOW_KEYS_BUTTON = GLFW_KEY_F4;
static bool takeScreenshot = false, takeUiScreenshot = false;
static bool showFps = true, showKeys = false;

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == SHOW_UI_BUTTON) {
		if (action == GLFW_PRESS && !uiButtonPressed) {
			uiButtonPressed = true;
			showUi = !showUi;
		} else if (action == GLFW_RELEASE && uiButtonPressed) {
			uiButtonPressed = false;
		}
	}
	if (key == SS_BUTTON) {
		if (action == GLFW_PRESS && !ssPressed) {
			ssPressed = true;
			takeScreenshot = true;
		} else if (action == GLFW_RELEASE && ssPressed) {
			ssPressed = false;
		}
	}
	if (key == SS_UI_BUTTON) {
		if (action == GLFW_PRESS && !ssPressed) {
			ssPressed = true;
			takeUiScreenshot = true;
		} else if (action == GLFW_RELEASE && ssPressed) {
			ssPressed = false;
		}
	}
	if (key == FPS_BUTTON) {
		if (action == GLFW_PRESS && !fpsPressed) {
			fpsPressed = true;
			showFps = !showFps;
		} else if (action == GLFW_RELEASE && fpsPressed) {
			fpsPressed = false;
		}
	}
	if (key == SHOW_KEYS_BUTTON) {
		if (action == GLFW_PRESS && !keysPressed) {
			keysPressed = true;
			showKeys = !showKeys;
		} else if (action == GLFW_RELEASE && keysPressed) {
			keysPressed = false;
		}
	}
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
	gameManager->mouse_scroll_callback(window, xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	gameManager->mouse_button_callback(window, butt, action, mods);
	ImGui_ImplGlfw_MouseButtonCallback(window, butt, action, mods);
}

int main(int argc, char** argv) {
	try {
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::trace);
		console_sink->set_pattern("[%Y-%m-%d %T.%e][%^%l%$][%@][%!] %v");
		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt", true);
		file_sink->set_level(spdlog::level::trace);
		file_sink->set_pattern("[%Y-%m-%d %T.%e][%^%l%$][%@][%!] %v");
		spdlog::set_default_logger(std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({ console_sink, file_sink })));
		spdlog::set_level(spdlog::level::trace);

	} catch (const spdlog::spdlog_ex& ex) {
		printf("Failed to initialize the logger! %s\n", ex.what());
		return 1;
	}

	SPDLOG_DEBUG("Logger initialized! Warming up...");

	gameManager = GameManager::getInstance();
	assetManager = AssetManager::getInstance();
	serializer = Serializer::getInstance();

	VideoSettings videoSettings = OptionsScene::loadVideoSettings();

	bool windowSizeDefined = false;

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
					videoSettings.windowWidth = target;
					windowSizeDefined = true;
					expectedWidth = false;
				} else {
					videoSettings.windowHeight = target;
					windowSizeDefined = true;
					expectedHeight = false;
				}
			}
		} else {
			if (strcmp("-fullscreen", arg) == 0) {
				videoSettings.windowType = Fullscreen;
			} else if (strcmp("-windowed", arg) == 0) {
				videoSettings.windowType = Windowed;
			} else if (strcmp("-borderless", arg) == 0) {
				videoSettings.windowType = WindowedBorderless;
			} else if (strcmp("-width", arg) == 0) {
				expectedWidth = true;
			} else if (strcmp("-height", arg) == 0) {
				expectedHeight = true;
			} else if (strcmp("-fs_borderless", arg) == 0) {
				videoSettings.windowType = FullscreenBorderless;
			}
		}
	}

	OptionsScene::setVideoSettings(videoSettings);

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

	float screenWidth = videoSettings.windowWidth, screenHeight = videoSettings.windowHeight;

	if (videoSettings.windowType == ExclusiveFullscreen || videoSettings.windowType == Fullscreen || videoSettings.windowType == FullscreenBorderless) {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *mode = glfwGetVideoMode(monitor);
		screenWidth = mode->width;
		screenHeight = mode->height;
		if (!windowSizeDefined && !videoSettings.windowSizeDefined) {
			videoSettings.windowWidth = screenWidth;
			videoSettings.windowHeight = screenHeight;
		}
	}

	gameManager->updateWindowSize(videoSettings.windowWidth, videoSettings.windowHeight, screenWidth, screenHeight, false);

	// Create window with graphics context
	GLFWwindow* window;
	switch (videoSettings.windowType) {
		case ExclusiveFullscreen:
		{
			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			window = glfwCreateWindow(screenWidth, screenHeight, "Tearoom", monitor, nullptr);
		}
		break;
		case FullscreenBorderless:
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			window = glfwCreateWindow(screenWidth, screenHeight, "Tearoom", nullptr, nullptr);
			break;
		case Fullscreen:
			window = glfwCreateWindow(screenWidth, screenHeight, "Tearoom", glfwGetPrimaryMonitor(), nullptr);
			break;
		case WindowedBorderless:
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			window = glfwCreateWindow(screenWidth, screenHeight, "Tearoom", nullptr, nullptr);
			break;
		case Windowed:
			window = glfwCreateWindow(screenWidth, screenHeight, "Tearoom", nullptr, nullptr);
			break;
		default:
			SPDLOG_ERROR("Unknown window type provided!");
			throw std::exception("Unknown window type provided.");
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
		SPDLOG_ERROR("Failed to initialize OpenGL loader!");
		return 1;
	}
	CHECK_GL_ERROR();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, false);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::StyleColorsDark();

	srand(time(nullptr));

	glfwMakeContextCurrent(window);

	if (!std::experimental::filesystem::is_directory("Screenshots") || !std::experimental::filesystem::exists("Screenshots")) {
		std::experimental::filesystem::create_directory("Screenshots");
	}

	CHECK_GL_ERROR();

	glFrontFace(GL_CCW);
	CHECK_GL_ERROR();
	glEnable(GL_CULL_FACE);
	CHECK_GL_ERROR();
	glCullFace(GL_BACK);
	CHECK_GL_ERROR();
	glEnable(GL_BLEND);
	CHECK_GL_ERROR();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	CHECK_GL_ERROR();
	//glEnable(GL_ALPHA_TEST);
	//CHECK_GL_ERROR();
	//glAlphaFunc(GL_GREATER, 0.01f);
	//CHECK_GL_ERROR();
	glEnable(GL_DEPTH_CLAMP);
	CHECK_GL_ERROR();

	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	gameManager->setCursorLocked(false);

	//int imgWidth, imgHeight, imgChannels;
	//unsigned char* imgData = stbi_load("res/ui/Cursor.png", &imgWidth, &imgHeight, &imgChannels, 0);
	//GLFWimage image;
	//image.width = 60;
	//image.height = 60;
	//image.pixels = imgData;
	//GLFWcursor *cursor = glfwCreateCursor(&image, 0, 0);
	//glfwSetCursor(window, cursor);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, keyboard_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetCharCallback(window, char_callback);
	CHECK_GL_ERROR();

	QuadData dat = UiTexturedElement::createFullscreenTexturedQuad();

	const glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);

	serializer->setup();
	gameManager->setup();
	SoundSystem::loadSounds();

	gameManager->updateWindowSize(videoSettings.windowWidth, videoSettings.windowHeight, screenWidth, screenHeight);

	GameFramebuffers framebuffers = gameManager->getFramebuffers();

	PostProcessingShader *postProcessingShader = dynamic_cast<PostProcessingShader*>(assetManager->getShader(STPostProcessing));
	postProcessingShader->use();
	postProcessingShader->setInt("scene", 0);
	for (int i = 0; i < BLOOM_TEXTURES; i++) {
		postProcessingShader->setInt(("bloom[" + std::to_string(i) + "]").c_str(), i + 1);
	}
	postProcessingShader->setWindowSize(videoSettings.windowWidth, videoSettings.windowHeight);
	postProcessingShader->setVec2("screenSize", glm::vec2(videoSettings.windowWidth, videoSettings.windowHeight));

	Shader *blurShader = assetManager->getShader(STBlur);

	UiColorPlane *fpsPlane = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.9f), glm::vec2(0.0f, 0.0f), glm::vec2(0.08f * UI_REF_WIDTH, 0.04f * UI_REF_HEIGHT), TopLeft);
	glm::vec2 planeCenter = fpsPlane->getPosition();
	planeCenter.x += fpsPlane->getSize().x / 2.0f;
	planeCenter.y += fpsPlane->getSize().y / 2.0f;
	UiText *fpsText = new UiText(planeCenter, fpsPlane->getSize(), "FPS: -", glm::vec3(1.0f, 1.0f, 1.0f), None);
	fpsPlane->addChild(fpsText);

	fpsPlane->updateDrawData();

	UiPlane *keysPlane = nullptr;

	Shader* fpsPlaneShader = assetManager->getShader(fpsPlane->getShaderType()), *fpsTextShader = assetManager->getShader(fpsText->getShaderType()), *screenTextShader = assetManager->getShader(STScreenTexture),
	*uiTexShader = assetManager->getShader(STUiTexture);

	SPDLOG_DEBUG("Starting the game loop!");

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		if (showUi && !takeUiScreenshot) {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}
		// Rendering
		static double currentTime, lastTime = 0.0, timeDelta, deltaSum = 100.0f;
		static int lastFps = 0;
		currentTime = glfwGetTime();
		timeDelta = currentTime - lastTime;
		lastTime = currentTime;
		lastFps = static_cast<int>(1.0 / timeDelta);
		deltaSum += timeDelta;
		if (deltaSum >= 0.5f) {   //update FPS text every second
			fpsText->setText("FPS: " + std::to_string(lastFps));
			deltaSum = 0.0f;
		}
		timeDelta <= 0.5 ? timeDelta : timeDelta = 1.0 / 120; //for debugging game loop

		Profiler::getInstance()->update(timeDelta);
		Profiler::getInstance()->clearFrameData();
		CHECK_GL_ERROR();
		gameManager->update(timeDelta);
		CHECK_GL_ERROR();
		glEnable(GL_DEPTH_TEST);
		// Render to a separate framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.main.fbo);
		glViewport(0, 0, videoSettings.windowWidth, videoSettings.windowHeight);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		CHECK_GL_ERROR();
		Profiler::getInstance()->startCountingTime();
		gameManager->render();
		CHECK_GL_ERROR();
		Profiler::getInstance()->addMeasure("Render calculations");

		glDisable(GL_DEPTH_TEST);

		if (postProcessingShader->isBloomEnabled()) {
			for (int i = 0; i < BLOOM_TEXTURES; i++) {
				screenTextShader->use();
				BloomFramebuffer bl = framebuffers.bloom[i];
				glViewport(0, 0, bl.width, bl.height);
				glBindFramebuffer(GL_FRAMEBUFFER, bl.rescaler.fbo);
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glBindTexture(GL_TEXTURE_2D, framebuffers.main.textures[1]);
				CHECK_GL_ERROR();
				dat.render();
				CHECK_GL_ERROR();
				blurShader->use();
				glBindFramebuffer(GL_FRAMEBUFFER, bl.horizontal.fbo);
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glBindTexture(GL_TEXTURE_2D, bl.rescaler.texture);
				CHECK_GL_ERROR();
				blurShader->setBool("horizontal", true);
				dat.render();
				CHECK_GL_ERROR();
				glBindFramebuffer(GL_FRAMEBUFFER, bl.output.fbo);
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glBindTexture(GL_TEXTURE_2D, bl.horizontal.texture);
				CHECK_GL_ERROR();
				blurShader->setBool("horizontal", false);
				dat.render();
				CHECK_GL_ERROR();
			}
		}
		// Render to the default framebuffer (screen) with post-processing
		Profiler::getInstance()->startCountingTime();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		CHECK_GL_ERROR();
		postProcessingShader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebuffers.main.textures[0]);
		CHECK_GL_ERROR();
		for (int i = 0; i < BLOOM_TEXTURES; i++) {
			glActiveTexture(GL_TEXTURE1 + i);
			glBindTexture(GL_TEXTURE_2D, framebuffers.bloom[i].output.texture);
			CHECK_GL_ERROR();
		}
		dat.render();
		CHECK_GL_ERROR();
		Profiler::getInstance()->addMeasure("PostProcessing");
		if (showUi && !takeUiScreenshot) {
			Profiler::getInstance()->startCountingTime();
			glViewport(0, 0, videoSettings.windowWidth, videoSettings.windowHeight);
			CHECK_GL_ERROR();
			gameManager->renderUi();
			CHECK_GL_ERROR();
			if (showFps) {
				fpsPlaneShader->use();
				fpsPlane->render(fpsPlaneShader);
				fpsTextShader->use();
				fpsText->render(fpsTextShader);
			}
			CHECK_GL_ERROR();
			if(showKeys && keysPlane != nullptr) {
				uiTexShader->use();
				keysPlane->render(uiTexShader);
			}
			CHECK_GL_ERROR();
			Profiler::getInstance()->addMeasure("UI Render");
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			CHECK_GL_ERROR();
		}

		if (takeScreenshot || takeUiScreenshot) {
			if (takeUiScreenshot) {
				SPDLOG_DEBUG("Creating a new screenshot without UI...");
			} else {
				SPDLOG_DEBUG("Creating a new standard screenshot...");
			}
			int totalSize = 3 * videoSettings.windowWidth * videoSettings.windowHeight;
			unsigned char *data = new unsigned char[totalSize];
			glReadPixels(0, 0, videoSettings.windowWidth, videoSettings.windowHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
			int width = videoSettings.windowWidth * 3;
			for (int i = 0; i < width; ++i) {
				for (int j = 0; j < static_cast<int>(videoSettings.windowHeight / 2); ++j) {
					int target = videoSettings.windowHeight - 1 - j;
					unsigned char temp = data[target*width + i];
					data[target*width + i] = data[j*width + i];
					data[j*width + i] = temp;
				}
			}
			time_t rawtime;
			char buffer[80];
			time(&rawtime);
			struct tm* timeinfo = localtime(&rawtime);
			strftime(buffer, sizeof(buffer), "%d-%m-%Y_%H-%M-%S", timeinfo);
			std::string timeString(buffer);
			stbi_write_jpg(("Screenshots\\screenshot_" + timeString + ".jpg").c_str(), videoSettings.windowWidth, videoSettings.windowHeight, 3, data, 100);
			SPDLOG_DEBUG(("Saved a new screenshot to 'screenshot_" + timeString + ".jpg'!").c_str());
			takeScreenshot = takeUiScreenshot = false;
			delete[] data;
		}

		CHECK_GL_ERROR();
		glfwSwapBuffers(window);

		bool loading = !assetManager->isLoaded();
		if (loading) {
			assetManager->loadNextPendingResource();
		}
		if (loading && assetManager->isLoaded()) {
			keysPlane = new UiPlane("res/textures/keyboardHints.PNG", glm::vec2(0.0f, 0.0f), glm::vec2(440.0f, 781.0f)* 0.7f, TopLeft);
			gameManager->goToMenu();
			SoundSystem::restartMusic();
		}
	}

	SPDLOG_DEBUG("Game loop closed! Shutting down...");

	delete gameManager;
	delete assetManager;
	delete LightManager::getInstance();
	dat.dispose();
	glfwDestroyWindow(window);
	//glfwDestroyCursor(cursor);
	glfwTerminate();

	return 0;
}
