#include "MeshText.h"
#include "Serialization/DataSerializer.h"

MeshText::MeshText(glm::vec4 color, std::string text) : MeshSimple(color, STText), text(text) { }

void MeshText::updateText(std::string text) {
	this->text = text;
}

void MeshText::draw(Shader* shader, glm::mat4 world) {
	static glm::mat4 *rot = nullptr;
	if(rot == nullptr) {
		rot = new glm::mat4(1.0f);
		*rot = rotate(*rot, static_cast<float>(M_PI), glm::vec3(0.0f, 0.0f, 1.0f));
		*rot = rotate(*rot, static_cast<float>(M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
		*rot = scale(*rot, glm::vec3(0.01f));
	}
	MeshSimple::draw(shader, world * *rot);
	getTextRenderer()->renderText(shader, text, 1.0f, 1.0f, 1.0f, 1.0f, Center);
}

SerializableType MeshText::getSerializableType() {
	return SMeshText;
}

Json::Value MeshText::serialize(Serializer* serializer) {
	Json::Value root = MeshSimple::serialize(serializer);
	root["text"] = text;
	return root;
}

void MeshText::deserialize(Json::Value& root, Serializer* serializer) {
	MeshSimple::deserialize(root, serializer);
	text = root.get("text", text).asString();
}

void MeshText::renderGui() {
	MeshSimple::renderGui();
	ImGui::Text(("Current text: '" + text + "'").c_str());
	static const int BUFF_SIZE = 50;
	static char buff[BUFF_SIZE] = "";
	if (buff[0] == '\0') {
		for (int i = 0; i < std::min(BUFF_SIZE, static_cast<int>(text.length())); i++) {
			buff[i] = text[i];
		}
	}
	ImGui::InputText("Target text", buff, IM_ARRAYSIZE(buff));
	std::string newName(Global::trim(buff));
	if (newName.length() > 0 && newName != text && ImGui::Button("Apply text")) {
		updateText(newName);
		buff[0] = '\0';
	}
}

TextRenderer* MeshText::getTextRenderer() {
	static TextRenderer *tr = nullptr;
	if(tr == nullptr) {
		tr = AssetManager::getInstance()->getTextRenderer();
	}
	return tr;
}
