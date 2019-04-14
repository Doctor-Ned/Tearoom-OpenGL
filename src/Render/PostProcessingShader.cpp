#include "PostProcessingShader.h"

PostProcessingShader::PostProcessingShader(char * vertexPath, char * fragmentPath) : Shader(vertexPath, fragmentPath) {
	use();
	setExposure(exposure);
	setGamma(gamma);
	setHdr(useHdr);
	setBloom(useBloom);
	setAntialiasing(useAntialiasing);
}

void PostProcessingShader::setExposure(float exposure) {
	setFloat("exposure", exposure);
	this->exposure = exposure;
}

void PostProcessingShader::setHdr(bool enabled) {
	setBool("useHdr", enabled);
	this->useHdr = enabled;
}

void PostProcessingShader::setGamma(float gamma) {
	setFloat("gamma", gamma);
	this->gamma = gamma;
}

void PostProcessingShader::setBloom(bool enabled) {
	setBool("useBloom", enabled);
	this->useBloom = enabled;
}

void PostProcessingShader::setAntialiasing(bool enabled) {
	setBool("useAntialiasing", enabled);
	this->useAntialiasing = enabled;
}

void PostProcessingShader::setWindowSize(float width, float height) {
	setVec2("inverseScreenSize", glm::vec2(1.0f / width, 1.0f / height));
}

float PostProcessingShader::getExposure() {
	return exposure;
}

float PostProcessingShader::getGamma() {
	return gamma;
}

bool PostProcessingShader::isHdrEnabled() {
	return useHdr;
}

bool PostProcessingShader::isBloomEnabled() {
	return useBloom;
}

bool PostProcessingShader::isAntialiasingEnabled() {
	return useAntialiasing;
}
