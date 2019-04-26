#ifndef SKYBOX_H
#define SKYBOX_H

#include "Global.h"
#include "Render/Shader.h"
#include <string>
#include <vector>

class Skybox : public Serializable {
public:
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	Skybox(std::vector<std::string> faces);
	void draw(Shader *shader, glm::mat4 untranslatedView, glm::mat4 projection);
	void draw(Shader *shader, glm::mat4 untranslatedView, glm::mat4 projection, GLuint cubemap);
protected:
	Skybox() {}
	friend class Serializer;
	void setup();
	GLuint VAO;
	GLuint VBO;
	Shader *shader;
	std::vector<std::string> faces;
	GLuint textureId;
};

#endif
