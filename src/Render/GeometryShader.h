#ifndef GEOMETRYSHADER_H
#define GEOMETRYSHADER_H

#include "Render/Shader.h"

class GeometryShader : public Shader {
public:
	GeometryShader(char* vertexPath, char* geometryPath, char* fragmentPath);
protected:
	void createShaders() override;
	char *geometryPath;
};

#endif
