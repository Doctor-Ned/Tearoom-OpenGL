#pragma once

class Serializer;

enum SerializableType {
	SNone,
	SGraphNode
};

class Serializable {
public:
	virtual Json::Value serialize(Serializer *serializer) = 0;
	virtual void deserialize(Json::Value &root, Serializer *serializer) = 0;
	virtual SerializableType getSerializableType() = 0;
};