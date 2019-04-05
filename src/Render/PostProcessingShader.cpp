#include "PostProcessingShader.h"

PostProcessingShader::PostProcessingShader(char * vertexPath, char * fragmentPath) : Shader(vertexPath, fragmentPath) {}

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
	setBool("bloom", enabled);
	this->useBloom = enabled;
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
