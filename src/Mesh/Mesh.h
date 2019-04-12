#ifndef MESH_H
#define MESH_H

#include "Global.h"
#include "Render/Shader.h"
#include "Scene/AssetManager.h"

class Mesh {
public:
	void setShininess(float shininess);
	void setUseLight(bool useLight);
	bool getUseLight() const;
	float getShininess() const;
	virtual void setOpacity(float opacity);
	float getOpacity() const;
	virtual void draw(Shader *shader, glm::mat4 world);
	ShaderType getShaderType();
	GLuint getRenderMode();
	void setRenderMode(GLuint renderMode);
	~Mesh();
protected:
	Mesh(ShaderType shaderType, GLuint renderMode = GL_TRIANGLES);
	ShaderType shaderType;
	GLuint renderMode;
	GLuint VAO = 0;
	GLuint VBO = 0, EBO = 0;
	float shininess = 1.0f;
	bool useLight;
	bool castShadows;
	float opacity = 1.0f;
};

#endif
