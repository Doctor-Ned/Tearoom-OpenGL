#ifndef GEOMETRYSHADER_H
#define GEOMETRYSHADER_H

#include "Render/Shader.h"

class GeometryShader : public Shader {
public:
	GeometryShader(char* vertexPath, char* geometryPath, char* fragmentPath);
	void setPointSpaces(glm::mat4 pointSpaces[6]);
	void setPointPosition(glm::vec3 pos);
protected:
	void createShaders() override;
	char *geometryPath;
};

#endif
