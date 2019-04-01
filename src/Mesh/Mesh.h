#ifndef MESH_H
#define MESH_H

#include "Global.h"
#include "Render/Shader.h"
#include <string>
#include <vector>
#include "Scene/SceneManager.h"

class Mesh {
public:
	void draw(glm::mat4 world);
	void draw(glm::mat4 world, float scale);
	void setShininess(float shininess);
	void setUseLight(bool useLight);
	bool getUseLight() const;
	float getShininess() const;
	virtual void draw(Shader *shader, glm::mat4 world, float scale = 1.0f);
	virtual ShaderType getShaderType() = 0;
	~Mesh();
protected:
	Mesh();
	GLuint VAO = 0;
	GLuint VBO = 0, EBO = 0;
	float shininess = 1.0f;
	Shader *shader;
	bool useLight;
	bool castShadows;
};

#endif
