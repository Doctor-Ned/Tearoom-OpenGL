#pragma once
#include "Shader.h"

class PostProcessingShader : public Shader {
public:
	PostProcessingShader(char* vertexPath, char* fragmentPath);
	void setExposure(float exposure);
	void setHdr(bool enabled);
	void setGamma(float gamma);
	float getExposure();
	float getGamma();
	bool isHdrEnabled();
protected:
	float exposure;
	float gamma;
	bool useHdr;
};
