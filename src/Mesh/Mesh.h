#ifndef MESH_H
#define MESH_H

#include "Global.h"
#include "Render/Shader.h"
#include "Scene/AssetManager.h"

class Mesh {
public:
	void draw(glm::mat4 world);
	void setShininess(float shininess);
	void setUseLight(bool useLight);
	bool getUseLight() const;
	float getShininess() const;
	virtual void draw(Shader *shader, glm::mat4 world);
	virtual ShaderType getShaderType() = 0;
	GLuint getRenderMode();
	void setRenderMode(GLuint renderMode);
	~Mesh();
protected:
	Mesh(GLuint renderMode = GL_TRIANGLES);
	GLuint renderMode;
	GLuint VAO = 0;
	GLuint VBO = 0, EBO = 0;
	float shininess = 1.0f;
	Shader *shader;
	bool useLight;
	bool castShadows;
};

#endif
