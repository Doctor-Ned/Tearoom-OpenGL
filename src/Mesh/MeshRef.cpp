#include "MeshRef.h"
#include "SceneManager.h"

void MeshRef::regenEnvironmentMap(glm::mat4 model, std::function<void(glm::mat4, glm::mat4)> renderCallback, GLuint framebuffer) {
	drawToCubemap(environmentMap, getUnmodeledCenter() + glm::vec3(model[3]), fbo, rb, renderCallback, framebuffer);
}

void MeshRef::draw(Shader shader, glm::mat4 model, float scale) {
	Mesh::draw(shader, model, scale);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);
}

GLuint MeshRef::getEnvironmentMap() {
	return environmentMap;
}

MeshRef::MeshRef(Shader shader) : Mesh(shader) {
	glGenTextures(1, &environmentMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);
	glActiveTexture(GL_TEXTURE0);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, ENVMAP_SIZE.x, ENVMAP_SIZE.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenRenderbuffers(1, &rb);
	glBindRenderbuffer(GL_RENDERBUFFER, rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ENVMAP_SIZE.x, ENVMAP_SIZE.y);
	GLenum drawBufs[] = { GL_COLOR_ATTACHMENT0 };
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo);
	glDrawBuffers(1, drawBufs);

	glBindFramebuffer(GL_FRAMEBUFFER, SceneManager::getInstance().getFramebuffer());
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
