#ifndef MESH_H
#define MESH_H

#include "Global.h"
#include "Render/Shader.h"
#include "Scene/AssetManager.h"
#include "Serialization/Serializable.h"

class Mesh abstract: public Serializable {
public:
	void setShaderType(ShaderType shaderType);
	void setShininess(float shininess);
	void setUseLight(bool useLight);
	bool getUseLight() const;
	virtual bool isOpaque() const;
	bool isCulled() const;
	void setOpaque(bool opaque);
	virtual void setCulled(bool culled);
	float getShininess() const;
	virtual void setOpacity(float opacity);
	void drawSelf(Shader *shader, glm::mat4 world);
	float getOpacity() const;
	ShaderType getShaderType();
	GLuint getRenderMode();
	void setRenderMode(GLuint renderMode);
	virtual ~Mesh();
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	Mesh(ShaderType shaderType, GLuint renderMode = GL_TRIANGLES);
	virtual void draw(Shader *shader, glm::mat4 world);
	bool opaque = true, culled = true;
	ShaderType shaderType;
	GLuint renderMode;
	GLuint VAO = 0;
	GLuint VBO = 0, EBO = 0;
	float shininess = 1.0f;
	bool useLight;
	bool castShadows;
	float opacity = 1.0f;
	Mesh() : Mesh(STNone) {}
	friend class Serializer;
};

#endif
