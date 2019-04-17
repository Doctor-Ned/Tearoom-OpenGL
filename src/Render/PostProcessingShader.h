#pragma once
#include "Shader.h"

class PostProcessingShader : public Shader {
public:
	PostProcessingShader(char* vertexPath, char* fragmentPath);
	void setExposure(float exposure);
	void setHdr(bool enabled);
	void setGamma(float gamma);
	void setBloom(bool enabled);
	void setAntialiasing(bool enabled);
	void setWindowSize(float width, float height);
	float getExposure();
	float getGamma();
	bool isHdrEnabled();
	bool isBloomEnabled();
	bool isAntialiasingEnabled();
protected:
	float exposure = 0.85f;
	float gamma = 0.8f;
	bool useHdr = true;
	bool useBloom = true;
	bool useAntialiasing = true;
};
