#ifndef SKYBOX_H
#define SKYBOX_H

#include "Headers.h"
#include "Shader.h"
#include <string>
#include <vector>

class Skybox {
public:
	Skybox(Shader shader, std::vector<std::string> faces);
	void draw(glm::mat4 untranslatedView, glm::mat4 projection);
	void draw(glm::mat4 untranslatedView, glm::mat4 projection, GLuint cubemap);
protected:
	void setup();
	GLuint VAO;
	GLuint VBO;
	Shader shader;
	std::vector<std::string> faces;
	GLuint textureId;
};

#endif
