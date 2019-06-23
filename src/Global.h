#ifndef GLOBAL_H
#define GLOBAL_H

#define GLM_ENABLE_EXPERIMENTAL

#include "imgui.h"
#include "imgui_impl_glfw.h"         // Visual Studio says it's redundant, but IT IS NOT.
#include <glm/gtx/rotate_vector.hpp> // same applies to this one
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define UI_REF_WIDTH 1280
#define UI_REF_HEIGHT 720
#define UI_REF_CEN_X UI_REF_WIDTH/2
#define UI_REF_CEN_Y UI_REF_HEIGHT/2

#include <vector>
#include <string>

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <functional>
#include "json/json.h"
#include <xmmintrin.h>
#include "assimp/matrix4x4.h"
#define ENABLE_GL_ERROR_CHECK
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"
#include <csignal>

#ifdef ENABLE_GL_ERROR_CHECK
#define CHECK_GL_ERROR() do { GLuint error=glGetError(); if(error != GL_NO_ERROR) { SPDLOG_ERROR("Encountered OpenGL error {}!", error); std::raise(SIGINT); } } while(false)
#else
#define CHECK_GL_ERROR()
#endif

enum WindowType {
	ExclusiveFullscreen,
	Fullscreen,
	FullscreenBorderless,
	Windowed,
	WindowedBorderless
};

static const WindowType WindowTypes[5]{
	ExclusiveFullscreen,
	Fullscreen,
	FullscreenBorderless,
	Windowed,
	WindowedBorderless
};

static const std::string WindowTypeNames[5] = {
	"Full screen (exclusive)",
	"Full screen",
	"Full screen (borderless)",
	"Windowed",
	"Windowed (borderless)"
};

struct VideoSettings {
	WindowType windowType;
	float windowWidth;
	float windowHeight;
	bool windowSizeDefined;
};

union SSEfloat4 {
	union __m128 f4;
	glm::vec4 v;
};

struct Texture {
	GLuint id{};
	std::string path;
};

static const glm::ivec2 ENVMAP_SIZE(2048.0f, 2048.0f);

static const char *BTN_SHORT_IDLE = "res/ui/ButtonIdle.png", *BTN_SHORT_CLICKED = "res/ui/ButtonClicked.png", *BTN_SHORT_HOVER = "res/ui/ButtonHover.png",
*BTN_LONG_IDLE = "res/ui/ButtonLongIdle.png", *BTN_LONG_CLICKED = "res/ui/ButtonLongClicked.png", *BTN_LONG_HOVER = "res/ui/ButtonLongHover.png",
*BTN_TICK_IDLE = "res/ui/ButtonTickIdle.png", *BTN_TICK_CLICKED = "res/ui/ButtonTickClicked.png", *BTN_TICK_HOVER = "res/ui/ButtonTickHover.png";

static const float BASE_LONG_BTN_WIDTH = 0.3f, BASE_LONG_BTN_HEIGHT = 0.1f, BASE_BTN_SIZE = 0.1f;

class Global {
public:
	static std::string getExtension(const std::string &path);
	static std::string ltrim(const std::string &s);
	static std::string rtrim(const std::string &s);
	static std::string trim(const std::string &s);
	static std::string getReplace(const std::string &str, const std::string &from, const std::string &to);
	static std::string getReplaceAll(const std::string &str, const std::string &from, const std::string &to);
	static bool replace(std::string &str, const std::string &from, const std::string &to);
	static void replaceAll(std::string &str, const std::string &from, const std::string &to);
	static glm::vec3 degreesToRadians(glm::vec3 rotation);
	static glm::vec3 radiansToDegrees(glm::vec3 rotation);
	static glm::vec3 getScale(glm::mat4 matrix);
	static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from);
	static bool endsWith(std::string text, std::string end);
	static bool startsWith(std::string text, std::string start);
	static std::string jsonValueToString(Json::Value value);
	static std::string readFullFile(std::string path);
	static Json::Value readJsonFile(std::string path);
	static void saveToFile(std::string path, Json::Value value);
	static void saveToFile(std::string path, std::string content);
	static glm::vec4 planeEquationOfPoints(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	static float distanceBetweenParallelPlanes(glm::vec4 plane1, glm::vec4 plane2);
	static glm::vec3* createHorizontalTransformArray(const int width, const int length, const glm::vec2 min, const glm::vec2 max, const float yPosition = 0.0f);
	static void drawToCubemap(GLuint cubemap, glm::vec3 position, GLuint fbo, GLuint rb, const std::function<void(glm::mat4, glm::mat4)> renderCallback);
	static double remap(const double value, const double sourceMin, const double sourceMax, double targetMin, double targetMax, const bool revertTarget = false, const bool snapIfInvalid = true);
	static float remap(const float value, const float sourceMin, const float sourceMax, float targetMin, float targetMax, const bool revertTarget, const bool snapIfInvalid);
	static int remap(const int value, const int sourceMin, const int sourceMax, const int targetMin, const int targetMax, const bool revertTarget = false, const bool snapIfInvalid = true);
private:
	static const std::string WHITESPACE;
};
#endif
