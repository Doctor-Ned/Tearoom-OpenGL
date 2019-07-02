#include "MeshRef.h"
#include "Scene/GameManager.h"

MeshRef::MeshRef(bool reflective) : Mesh(reflective ? STReflect : STRefract) {
	CHECK_GL_ERROR();
	int oldFbo, oldRbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRbo);
	glGenTextures(1, &environmentMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);
	glActiveTexture(GL_TEXTURE0);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	CHECK_GL_ERROR();
	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, ENVMAP_SIZE.x, ENVMAP_SIZE.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		CHECK_GL_ERROR();
	}
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CHECK_GL_ERROR();
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	CHECK_GL_ERROR();
	rb = GameManager::getInstance()->createDepthRenderbuffer(ENVMAP_SIZE.x, ENVMAP_SIZE.y);
	GLenum drawBufs[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBufs);
	CHECK_GL_ERROR();
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);
	glBindRenderbuffer(GL_RENDERBUFFER, oldRbo);
	CHECK_GL_ERROR();
}

void MeshRef::draw(Shader *shader, glm::mat4 model) {
	Mesh::draw(shader, model);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);
}

void MeshRef::regenEnvironmentMap(glm::mat4 model, std::function<void(glm::mat4, glm::mat4)> renderCallback) {
	Global::drawToCubemap(environmentMap, getUnmodeledCenter() + glm::vec3(model[3]), fbo, rb, renderCallback);
}

GLuint MeshRef::getEnvironmentMap() const {
	return environmentMap;
}

SerializableType MeshRef::getSerializableType() {
	return SMeshRef;
}
