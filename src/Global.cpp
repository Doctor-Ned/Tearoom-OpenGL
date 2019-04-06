#include "Global.h"
#include "Scene/GameManager.h"

glm::vec3* Global::createHorizontalTransformArray(const int width, const int length, const glm::vec2 min, const glm::vec2 max,
	const float yPosition) {
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

void Global::drawToCubemap(GLuint cubemap, glm::vec3 position, GLuint fbo, GLuint rb,
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

double Global::remap(const double value, const double sourceMin, const double sourceMax, double targetMin, double targetMax,
	const bool revertTarget, const bool snapIfInvalid) {
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

int Global::remap(const int value, const int sourceMin, const int sourceMax, const int targetMin, const int targetMax, const bool revertTarget,
	const bool snapIfInvalid) {
	return static_cast<int>(remap(static_cast<double>(value), static_cast<double>(sourceMin), static_cast<double>(sourceMax), static_cast<double>(targetMin),
		static_cast<double>(targetMax),
		revertTarget, snapIfInvalid));
}