#ifndef MESH_H
#define MESH_H

#include "Global.h"
#include "Render/Shader.h"
#include "Scene/AssetManager.h"
#include "Serialization/Serializable.h"

class Mesh abstract: public Serializable, public GuiConfigurable {
public:
	ShaderType getShaderType();
	void setShaderType(ShaderType shaderType);
	bool getCastShadows();
	bool getUseLight() const;
	bool isCulled() const;
	virtual bool isOpaque() const;
	float getOpacity() const;
	float getEmissiveFactor() const;
	virtual void setCastShadows(bool castShadows);
	virtual void setUseLight(bool useLight);
	virtual void setCulled(bool culled);
	virtual void setEmissiveFactor(float emissiveFactor);
	virtual void setOpaque(bool opaque);
	virtual void setOpacity(float opacity);
	virtual void setDepthScale(float depth);
	void drawSelf(Shader *shader, glm::mat4 world);
	GLuint getRenderMode();
	virtual void setRenderMode(GLuint renderMode);
	virtual ~Mesh();
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	void setUiScale(float uiScale);
	float getUiScale();
protected:
	void renderGui() override;
	Mesh(ShaderType shaderType, GLuint renderMode = GL_TRIANGLES);
	virtual void draw(Shader *shader, glm::mat4 world);
	bool opaque = true, culled = true;
	ShaderType shaderType;
	GLuint renderMode;
	GLuint VAO = 0;
	GLuint VBO = 0, EBO = 0;
	bool useLight;
	bool castShadows;
	float opacity = 1.0f;
	float uiScale = 1.0f;
	float emissiveFactor = 1.0f;
	float depthScale = 0.03f;
	Mesh() : Mesh(STNone) {}
	friend class Serializer;
};

#endif
