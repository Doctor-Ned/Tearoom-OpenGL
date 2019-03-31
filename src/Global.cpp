#include "Global.h"
#include "Scene/SceneManager.h"

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

Texture Global::createTexture(const char* textureFile) {
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

GLuint Global::loadCubemap(std::vector<std::string> faces) {
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
		} else {
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

LightShadowData Global::getDirShadowData(int shadowWidth, int shadowHeight) {
	LightShadowData result;
	result.width = shadowWidth;
	result.height = shadowHeight;
	int oldFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);
	glGenFramebuffers(1, &result.fbo);
	glGenTextures(1, &result.texture);
	glBindTexture(GL_TEXTURE_2D, result.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, result.fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, result.texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);
	return result;
}

std::vector<LightShadowData> Global::getDirsShadowData(int count, int shadowWidth, int shadowHeight) {
	std::vector<LightShadowData> data;
	for (int i = 0; i < count; i++) {
		data.push_back(getDirShadowData(shadowWidth, shadowHeight));
	}
	return data;
}

LightShadowData Global::getSpotShadowData(int shadowWidth, int shadowHeight) {
	return getDirShadowData(shadowWidth, shadowHeight);
}

std::vector<LightShadowData> Global::getSpotsShadowData(int count, int shadowWidth, int shadowHeight) {
	std::vector<LightShadowData> data;
	for (int i = 0; i < count; i++) {
		data.push_back(getSpotShadowData(shadowWidth, shadowHeight));
	}
	return data;
}

LightShadowData Global::getPointShadowData(int shadowWidth, int shadowHeight) {
	LightShadowData result;
	result.width = shadowWidth;
	result.height = shadowHeight;
	int oldFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);
	glGenFramebuffers(1, &result.fbo);
	glGenTextures(1, &result.texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, result.texture);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, result.fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, result.texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);
	return result;
}

std::vector<LightShadowData> Global::getPointsShadowData(int count, int shadowWidth, int shadowHeight) {
	std::vector<LightShadowData> data;
	for (int i = 0; i < count; i++) {
		data.push_back(getPointShadowData(shadowWidth, shadowHeight));
	}
	return data;
}