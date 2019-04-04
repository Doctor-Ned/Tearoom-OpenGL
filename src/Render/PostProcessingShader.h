#pragma once
#include "Shader.h"

class PostProcessingShader : public Shader {
public:
	PostProcessingShader(char* vertexPath, char* fragmentPath);
	void setExposure(float exposure);
	void setHdr(bool enabled);
};
