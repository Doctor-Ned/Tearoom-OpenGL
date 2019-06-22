#include "Skybox.h"
#include "Scene/AssetManager.h"

SerializableType Skybox::getSerializableType() {
	return SSkybox;
}

Json::Value Skybox::serialize(Serializer* serializer) {
	Json::Value root;
	for(int i=0;i<6;i++) {
		root["faces"][i] = faces[i];
	}
	return root;
}

void Skybox::deserialize(Json::Value& root, Serializer* serializer) {
	if(root["faces"].type() != Json::arrayValue || root["faces"].size() < 6) {
		SPDLOG_ERROR("Invalid skybox deserialization data!");
		
		std::exception("Invalid skybox deserialization data!");
	}
	for(int i=0;i<6;i++) {
		faces.push_back(root["faces"][i].asString());
	}
	setup();
}

Skybox::Skybox(std::vector<std::string> faces) : faces(faces) {
	setup();
}

void Skybox::draw(Shader *shader, glm::mat4 untranslatedView, glm::mat4 projection) {
	draw(shader, untranslatedView, projection, textureId);
}

void Skybox::draw(Shader *shader, glm::mat4 untranslatedView, glm::mat4 projection, GLuint cubemap) {
	glDepthFunc(GL_LEQUAL);
	shader->use();
	shader->setView(untranslatedView);
	shader->setProjection(projection);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Skybox::~Skybox() {
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void Skybox::setup() {
	textureId = AssetManager::getInstance()->createCubemap(faces);

	GLfloat vertices[] = {
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GLfloat), &vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

}
