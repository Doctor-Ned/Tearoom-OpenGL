#include "MeshTexture.h"
#include "Scene/Scenes/EditorScene.h"

SerializableType MeshTexture::getSerializableType() {
	return SMeshTexture;
}

Json::Value MeshTexture::serialize(Serializer* serializer) {
	Json::Value root = MeshProperty::serialize(serializer);
	root["texture"] = texture.path;
	return root;
}

void MeshTexture::deserialize(Json::Value& root, Serializer* serializer) {
	MeshProperty::deserialize(root, serializer);
	texture = AssetManager::getInstance()->getTexture(root.get("texture", "").asString());
}

void MeshTexture::renderGui() {
	MeshProperty::renderGui();
	ImGui::Text(("Texture: " + texture.path).c_str());
	EditorScene *editor = GameManager::getInstance()->getEditorScene();
	if (editor != nullptr && editor->textureSelectionCallback == nullptr) {
		ImGui::SameLine();
		if (ImGui::Button("Change...")) {
			editor->textureSelectionCallback = [this](Texture t) {
				texture = t;
			};
		}
	}
}

void MeshTexture::draw(Shader *shader, glm::mat4 world) {
	MeshProperty::draw(shader, world);
}

MeshTexture::MeshTexture() : MeshProperty(STTexture) { }
