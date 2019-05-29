#pragma once

#include "Global.h"
#include "MeshSimple.h"

class MeshText : public MeshSimple {
public:
	MeshText(glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), std::string text = "");
	void updateText(std::string text);
	void draw(Shader* shader, glm::mat4 world) override;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	void renderGui() override;
	static TextRenderer *getTextRenderer();
	std::string text;
	friend class Serializer;
};