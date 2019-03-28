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
	bool getUseLight();
	virtual void draw(Shader shader, glm::mat4 world, float scale = 1.0f);
	virtual void drawGui(bool autoUpdate = true);
	virtual void setShader(Shader shader);
	Shader *getShader();
	~Mesh();
protected:
	Mesh(Shader shader, std::vector<unsigned int> indices);
	Mesh(Shader shader);
	GLuint VAO;
	GLuint VBO, EBO;
	float shininess = 1.0f;
	std::vector<unsigned int> indices;
	Shader shader;
	bool useLight = true;
	virtual void setupMesh() = 0;
};

#endif
