#include "Global.h"
#include <cstdio>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <GLFW/glfw3.h>
#include "freetype/ftparams.h"
#include <ctime>
#include "Scene/SceneManager.h"
#include "Ui/UiElement.h"
#include "Render/Shader.h"
#include "Scene/MiszukScene.h"
#include "Scene/TestScene.h"

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static SceneManager* sceneManager;

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	sceneManager->keyboard_callback(window, key, scancode, action, mods);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	sceneManager->mouse_callback(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	sceneManager->mouse_button_callback(window, butt, action, mods);
}


int main(int argc, char** argv) {
	sceneManager = SceneManager::getInstance();
	bool borderless = false;
	bool fullscreen = false;

	float windowWidth = 1280.0f, windowHeight = 720.0f;

	bool expectedWidth = false, expectedHeight = false;

	for (int i = 0; i < argc; i++) {
		char* arg = argv[i];

		if (expectedWidth || expectedHeight) {
			int target = 0;
			bool correct = true;
			int count = 0;
			for (count = 0; arg[count] != '\0'; count++) {
				if(arg[count] < '0' || arg[count] > '9') {
					correct = false;
					break;
				}
			}
			if(correct) {
				int multiplier = 1;
				for(int j=0;j<count;j++) {
					target += multiplier * (arg[count - 1 - j] - '0');
					multiplier *= 10;
				}
				if(expectedWidth) {
					windowWidth = target;
					expectedWidth = false;
				} else {
					windowHeight = target;
					expectedHeight = false;
				}
			}
		} else {
			if(strcmp("-fullscreen", arg) == 0) {
				fullscreen = true;
			} else if (strcmp("-windowed", arg) == 0) {
				fullscreen = false;
			} else if (strcmp("-borderless", arg) == 0) {
				borderless = true;
			} else if (strcmp("-width", arg) == 0) {
				expectedWidth = true;
			} else if (strcmp("-height", arg) == 0) {
				expectedHeight = true;
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

	if (fullscreen) {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *mode = glfwGetVideoMode(monitor);
		screenWidth = mode->width;
		screenHeight = mode->height;
	}

	sceneManager->updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);

	// Create window with graphics context
	GLFWwindow* window;
	if (borderless) {
		if(fullscreen) {
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
	glfwSwapInterval(1); // Enable vsync

	sceneManager->setWindow(window);

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

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::StyleColorsDark();

	srand(time(nullptr));

	sceneManager->setup();

	//glEnable(GL_DEPTH_TEST); it gets enabled later.
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, keyboard_callback);

	GLuint fbo, texture, rbo;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	GLenum status;
	if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "glCheckFramebufferStatus: error %u", status);
		return 0;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	sceneManager->setFramebuffer(fbo);

	Shader post_processing("Post/postProcessingVS.glsl", "Post/postProcessingFS.glsl");

	const glm::vec4 clear_color(0.2f, 0.0f, 0.6f, 1.0f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// Rendering
		static double currentTime, lastTime = 0.0, timeDelta;
		currentTime = glfwGetTime();
		timeDelta = currentTime - lastTime;
		lastTime = currentTime;
		timeDelta <= 1.0 / 60.0 ? timeDelta : timeDelta = 1.0 / 60.0; //for debugging game loop
		sceneManager->update(timeDelta);
		glfwMakeContextCurrent(window);

		// Render to a separate framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		sceneManager->render();

		// Render to the default framebuffer (screen) with post-processing
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		post_processing.use();
		ImGui::Render();
		glBindVertexArray(vao);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexBuffer(0, vbo, 0, sizeof(UiTextureVertex));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glUseProgram(0);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}
	delete sceneManager;
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteRenderbuffers(1, &rbo);
	glDeleteTextures(1, &texture);
	glDeleteFramebuffers(1, &fbo);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
