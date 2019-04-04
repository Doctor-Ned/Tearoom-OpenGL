#include "PostProcessingShader.h"

PostProcessingShader::PostProcessingShader(char * vertexPath, char * fragmentPath) : Shader(vertexPath, fragmentPath) {}

void PostProcessingShader::setExposure(float exposure) {
	setFloat("exposure", exposure);
}

void PostProcessingShader::setHdr(bool enabled) {
	setBool("useHdr", enabled);
}
