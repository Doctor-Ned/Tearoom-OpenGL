#include "Mesh.h"
#include "Scene/GameManager.h"
#include "Scene/Scenes/Scene.h"

void Mesh::setShaderType(ShaderType shaderType) {
	this->shaderType = shaderType;
}

void Mesh::setShininess(float shininess) {
	this->shininess = shininess;
}

void Mesh::setUseLight(bool useLight) {
	this->useLight = useLight;
}

void Mesh::setCastShadows(bool castShadows) {
	this->castShadows = castShadows;
}

bool Mesh::getCastShadows() {
	return castShadows;
}

bool Mesh::getUseLight() const {
	return useLight;
}

bool Mesh::isOpaque() const {
	return opacity > 0.99f && opaque;
}

bool Mesh::isCulled() const {
	return culled;
}

void Mesh::setOpaque(bool opaque) {
	this->opaque = opaque;
}

void Mesh::setCulled(bool culled) {
	this->culled = culled;
}

float Mesh::getShininess() const {
	return shininess;
}

void Mesh::draw(Shader *shader, glm::mat4 world) {
	//shader->use();
	shader->setShininess(shininess);
	shader->setOpacity(opacity);
	shader->setModel(world);
	shader->setUseLight(useLight);
	shader->setUseSpecular(false);
	shader->setCastShadows(castShadows);
}

ShaderType Mesh::getShaderType() {
	return shaderType;
}

GLuint Mesh::getRenderMode() {
	return renderMode;
}
void Mesh::setRenderMode(GLuint renderMode) {
	this->renderMode = renderMode;
}

void Mesh::setOpacity(float opacity) {
	this->opacity = opacity;
}

void Mesh::drawSelf(Shader* shader, glm::mat4 world) {
	if (culled) {
		draw(shader, world);
	} else {
		glDisable(GL_CULL_FACE);
		draw(shader, world);
		glEnable(GL_CULL_FACE);
	}
}

float Mesh::getOpacity() const {
	return opacity;
}

Mesh::~Mesh() {
	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
	}
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
	}
	if (EBO != 0) {
		glDeleteBuffers(1, &EBO);
	}
}

SerializableType Mesh::getSerializableType() {
	return SMesh;
}

Json::Value Mesh::serialize(Serializer* serializer) {
	Json::Value root;
	root["shaderType"] = static_cast<int>(shaderType);
	root["shininess"] = shininess;
	root["culled"] = culled;
	root["useLight"] = useLight;
	root["opaque"] = opaque;
	root["opacity"] = opacity;
	root["castShadows"] = castShadows;
	root["uiScale"] = uiScale;
	root["renderMode"] = static_cast<int>(renderMode);
	return root;
}

void Mesh::deserialize(Json::Value& root, Serializer* serializer) {
	setShaderType(static_cast<ShaderType>(root.get("shaderType", static_cast<int>(shaderType)).asInt()));
	setShininess(root.get("shininess", shininess).asFloat());
	setCulled(root.get("culled", culled).asBool());
	setUseLight(root.get("useLight", useLight).asBool());
	setOpaque(root.get("opaque", opaque).asBool());
	setCastShadows(root.get("castShadows", castShadows).asBool());
	setOpacity(root.get("opacity", opacity).asFloat());
	setUiScale(root.get("uiScale", uiScale).asFloat());
	setRenderMode(static_cast<GLenum>(root.get("renderMode", static_cast<int>(renderMode)).asInt()));
}

void Mesh::renderGui() {
	ImGui::Text(("Mesh type: " + SerializableTypeNames[getSerializableType()]).c_str());
	bool useLight = this->useLight,
		opaque = this->opaque,
		culled = this->culled,
		castShadows = this->castShadows;
	float opacity = this->opacity, uiScale=this->uiScale;

	ImGui::Checkbox("Use light", &useLight);
	ImGui::Checkbox("Cast shadows", &castShadows);
	ImGui::Checkbox("Opaque", &opaque);
	ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f);
	ImGui::Checkbox("Culled", &culled);
	ImGui::DragFloat("Scale in UI", &uiScale, 0.01f);
	if (useLight != this->useLight) {
		setUseLight(useLight);
	}
	if (castShadows != this->castShadows) {
		setCastShadows(castShadows);
	}
	if (opaque != this->opaque) {
		setOpaque(opaque);
	}
	if (opacity != this->opacity) {
		setOpacity(opacity);
	}
	if (culled != this->culled) {
		setCulled(culled);
	}
	if(uiScale != this->uiScale) {
		setUiScale(uiScale);
	}
}

void Mesh::setUiScale(float uiScale) {
	this->uiScale = uiScale;
}

float Mesh::getUiScale() {
	return uiScale;
}

Mesh::Mesh(ShaderType shaderType, GLuint renderMode) {
	this->renderMode = renderMode;
	this->shaderType = shaderType;
	useLight = true;
	castShadows = true;
}
