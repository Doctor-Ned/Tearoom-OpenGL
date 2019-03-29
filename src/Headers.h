#ifndef HEADERS_H
#define HEADERS_H

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

static const float WINDOW_WIDTH = 1280.0f, WINDOW_HEIGHT = 720.0f, WINDOW_CENTER_X = WINDOW_WIDTH / 2.0f,
                   WINDOW_CENTER_Y = WINDOW_HEIGHT / 2.0f;

static glm::vec3* createHorizontalTransformArray(const int width, const int length, const glm::vec2 min, const glm::vec2 max,
                                                 const float yPosition = 0.0f) {
	glm::vec3* result = new glm::vec3[width * length];
	const float firstX = width == 1 ? (max.x + min.x) / 2.0f : min.x;
	const float xStep = width == 1 ? 0.0f : (max.x - min.x) / static_cast<float>(width - 1);
	const float firstZ = length == 1 ? (max.y - min.y) / 2.0f : min.y;
	const float zStep = length == 1 ? 0.0f : (max.y - min.y) / static_cast<float>(length - 1);
	int counter = 0;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < length; j++) {
			result[counter++] = glm::vec3(i * xStep + firstX, yPosition, j * zStep + firstZ);
		}
	}
	return result;
}

struct Texture {
	GLuint id{};
	std::string path;
};

static const glm::ivec2 ENVMAP_SIZE(2048.0f, 2048.0f);

static void drawToCubemap(GLuint cubemap, glm::vec3 position, GLuint fbo, GLuint rb,
                          const std::function<void(glm::mat4, glm::mat4)> renderCallback, GLuint framebuffer) {

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glm::mat4 p = glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 100.0f);

	glm::vec3 targets[6] = {
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	};
	glm::vec3 ups[6] = {
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)
	};

	glViewport(0, 0, ENVMAP_SIZE.x, ENVMAP_SIZE.y);
	for (int i = 0; i < 6; i++) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

		glm::mat4 v = lookAt(position, position + targets[i], ups[i]);
		//v[0][2] *= -1.0f;
		//v[1][2] *= -1.0f;
		//v[2][2] *= -1.0f;
		//v[3][2] *= -1.0f;
		renderCallback(v, p);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

static Texture createTexture(const char* textureFile) {
	Texture texture;
	int imgWidth, imgHeight, imgChannels;
	unsigned char* imgData = stbi_load(textureFile, &imgWidth, &imgHeight, &imgChannels, 0);
	if (!imgData) {
		fprintf(stderr, "Failed to load texture from file \"%s\"!", textureFile);
		exit(1);
	}
	GLenum format = GL_RGB;
	if (imgChannels == 4) {
		format = GL_RGBA;
	}
	GLuint imgTexture;
	glGenTextures(1, &imgTexture);
	glBindTexture(GL_TEXTURE_2D, imgTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, imgData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imgData);
	texture.id = imgTexture;
	texture.path = textureFile;
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

static GLuint loadCubemap(std::vector<std::string> faces) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
			             data);
			stbi_image_free(data);
		}
		else {
			printf("Cubemap texture failed to load at path '%s'!\n", faces[i].c_str());
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

static double remap(const double value, const double sourceMin, const double sourceMax, double targetMin, double targetMax,
                    const bool revertTarget = false, const bool snapIfInvalid = true) {
	if (value < sourceMin || value > sourceMax) {
		if (snapIfInvalid) {
			return value < sourceMin ? targetMin : targetMax;
		}
	}
	double result = (value - sourceMin) / (sourceMax - sourceMin) * (targetMax - targetMin) + targetMin;
	if (revertTarget) {
		result = targetMax - result + targetMin;
	}

	return result;
}

static int remap(const int value, const int sourceMin, const int sourceMax, const int targetMin, const int targetMax, const bool revertTarget = false,
                 const bool snapIfInvalid = true) {
	return static_cast<int>(remap(static_cast<double>(value), static_cast<double>(sourceMin), static_cast<double>(sourceMax), static_cast<double>(targetMin),
	                              static_cast<double>(targetMax),
	                              revertTarget, snapIfInvalid));
}

#define MAX_LIGHTS_OF_TYPE 16                                   // this MUST be identical to the value from the shader

#define DIRLIGHT_SIZE (32 * sizeof(float))                        // 4 * vec4 + mat4

#define POINTLIGHT_SIZE (36 * sizeof(float))                      // 4 * float + 4 * vec4 + mat4

#define SPOTLIGHT_SIZE (44 * sizeof(float))                       // 5 * float + 5 * vec4 + mat4 + vec3

struct DirLight {
	glm::vec4 direction;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::mat4 model;
};

struct PointLight {
	glm::vec4 position;
	float constant;
	float linear;
	float quadratic;
	float padding;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::mat4 model;
};

struct SpotLight {
	glm::vec4 position;
	glm::vec4 direction;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::mat4 model;
	glm::vec3 padding;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};

#endif
