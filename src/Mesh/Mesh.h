#ifndef MESH_H
#define MESH_H

#include "Headers.h"
#include "Render/Shader.h"
#include <string>
#include <vector>

class Mesh {
public:
	void draw(glm::mat4 world);
	void draw(glm::mat4 world, float scale);
	void setShininess(float shininess);
	void setUseLight(bool useLight);
	bool getUseLight() const;
	float getShininess() const;
	virtual void draw(Shader *shader, glm::mat4 world, float scale = 1.0f);
	void setShader(Shader *shader);
	Shader *getShader() const;
	~Mesh();
protected:
	Mesh(Shader *shader);
	GLuint VAO;
	GLuint VBO, EBO;
	float shininess = 1.0f;
	Shader *shader;
	bool useLight = true;
};

#endif
