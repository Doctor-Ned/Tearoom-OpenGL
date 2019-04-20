#pragma once

#include <string>
#include <map>
#include "Global.h"

class GraphNode;

enum SerializableType {
	SNone,
	SGraphNode
};

struct SerializablePointer {
	SerializablePointer() : id(-1), type(SNone), object(nullptr) {}
	SerializablePointer(SerializableType type, void* object) : id(-1), type(type), object(object) {}
	SerializablePointer(int id, SerializableType type, void* object) : id(id), type(type), object(object) {}
	int id;
	SerializableType type;
	void *object;
};

class Serializer {
public:
	static Serializer *getInstance();
	void setup();
	Serializer(Serializer const&) = delete;
	void operator=(Serializer const&) = delete;
	void saveScene(GraphNode *scene, const std::string& name);
	GraphNode *loadScene(const std::string& name);
private:
	void *getPointer(int id);
	int getId(void *pointer);
	int idCounter;
	std::map<void*, int> ids;
	void serialize(SerializablePointer ser, Json::Value &root);
	GraphNode *deserializeScene(Json::Value &root);
	SerializablePointer deserialize(Json::Value &root);
	void loadScenes();
	Serializer() = default;
	const std::string SCENES_DIR = "Scenes";
	const std::string FORMAT = ".scene";
	const std::string OLD_FORMAT = ".oldscene";
	std::map<std::string, std::string> scenes;
};
