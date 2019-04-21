#ifndef MESHTEXTURE_H
#define MESHTEXTURE_H

#include "Mesh.h"
#include "Scene/AssetManager.h"

struct TextureVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class MeshTexture abstract: public Mesh {
public:
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer *serializer) override;
	void deserialize(Json::Value &root, Serializer *serializer) override;
protected:
	void draw(Shader *shader, glm::mat4 world) override;
	MeshTexture();
	Texture texture;
};

#endif
