#pragma once
#include "Shader.h"

class PostProcessingShader : public Shader {
public:
	PostProcessingShader(char* vertexPath, char* fragmentPath);
	void setExposure(float exposure);
	void setHdr(bool enabled);
	void setGamma(float gamma);
	void setBloom(bool enabled);
	float getExposure();
	float getGamma();
	bool isHdrEnabled();
	bool isBloomEnabled();
protected:
	float exposure = 1.0f;
	float gamma = 1.0f;
	bool useHdr = true;
	bool useBloom = true;
};
