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

#include <glm/gtc/matrix_transform.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

struct Texture {
	GLuint id{};
	std::string path;
};

static const glm::ivec2 ENVMAP_SIZE(2048.0f, 2048.0f);

static const char *BTN_SHORT_IDLE = "res/ui/ButtonIdle.png", *BTN_SHORT_CLICKED = "res/ui/ButtonClicked.png", *BTN_SHORT_HOVER = "res/ui/ButtonHover.png",
*BTN_LONG_IDLE = "res/ui/ButtonLongIdle.png", *BTN_LONG_CLICKED = "res/ui/ButtonLongClicked.png", *BTN_LONG_HOVER = "res/ui/ButtonLongHover.png",
*BTN_TICK_IDLE = "res/ui/ButtonTickIdle.png", *BTN_TICK_CLICKED = "res/ui/ButtonTickClicked.png", *BTN_TICK_HOVER = "res/ui/ButtonTickHover.png";

static const float BASE_LONG_BTN_WIDTH = 0.3f, BASE_LONG_BTN_HEIGHT = 0.1f, BASE_BTN_SIZE = 0.1f;

static const float BASE_AMBIENT = 0.05f;

#define MAX_LIGHTS_OF_TYPE 4    // this MUST be identical to the value from the shader

#define DIRLIGHT_SIZE sizeof(DirLight)

#define POINTLIGHT_SIZE sizeof(PointLight)

#define SPOTLIGHT_SIZE sizeof(SpotLight)

struct DirLight {
	glm::mat4 lightSpace;
	glm::vec4 direction;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::mat4 model;
};

struct PointLight {
	float constant;
	float linear;
	float quadratic;
	float near_plane;
	glm::vec3 padding;
	float far_plane;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::mat4 model;
};

struct SpotLight {
	glm::mat4 lightSpace;
	glm::vec4 direction;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::mat4 model;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	glm::vec3 padding;
	float outerCutOff;
};

struct LightShadowData {
	GLuint fbo;
	GLuint texture;
	GLsizei width;
	GLsizei height;
};

class Global {
public:
	static glm::vec3* createHorizontalTransformArray(const int width, const int length, const glm::vec2 min, const glm::vec2 max, const float yPosition = 0.0f);
	static void drawToCubemap(GLuint cubemap, glm::vec3 position, GLuint fbo, GLuint rb, const std::function<void(glm::mat4, glm::mat4)> renderCallback, GLuint framebuffer);
	static Texture createTexture(const char* textureFile);
	static GLuint loadCubemap(std::vector<std::string> faces);
	static double remap(const double value, const double sourceMin, const double sourceMax, double targetMin, double targetMax, const bool revertTarget = false, const bool snapIfInvalid = true);
	static int remap(const int value, const int sourceMin, const int sourceMax, const int targetMin, const int targetMax, const bool revertTarget = false, const bool snapIfInvalid = true);
	static LightShadowData *getDirShadowData(int shadowWidth = 2048, int shadowHeight = 2048);
	static std::vector<LightShadowData*> getDirsShadowData(int count, int shadowWidth = 2048, int shadowHeight = 2048);
	static LightShadowData *getSpotShadowData(int shadowWidth = 2048, int shadowHeight = 2048);
	static std::vector<LightShadowData*> getSpotsShadowData(int count, int shadowWidth = 2048, int shadowHeight = 2048);
	static LightShadowData *getPointShadowData(int shadowWidth = 2048, int shadowHeight = 2048);
	static std::vector<LightShadowData*> getPointsShadowData(int count, int shadowWidth = 2048, int shadowHeight = 2048);
};

#endif
