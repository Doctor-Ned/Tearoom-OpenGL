#pragma once

#include <string>
#include <map>
#include "Global.h"
#include "Serializable.h"

class Scene;
class GraphNode;

struct SerializablePointer {
	SerializablePointer() : id(-1), object(nullptr) {}
	SerializablePointer(Serializable* object) : id(-1), object(object) {}
	SerializablePointer(int id, Serializable* object) : id(id), object(object) {}
	int id;
	Serializable* object;
};

class Serializer {
public:
	static Serializer *getInstance();
	void setup();
	Serializer(Serializer const&) = delete;
	void operator=(Serializer const&) = delete;
	void saveScene(Scene *scene, const std::string& name);
	Scene *loadScene(const std::string& name);
	Serializable *getPointer(int id);
	std::vector<std::string> getSceneNames();
	int getId(Serializable *pointer);
	Json::Value serialize(Serializable *ser);
	Json::Value serialize(SerializablePointer ser);
	SerializablePointer deserialize(Json::Value &root);
	void loadScenes();
private:
	void deserializeAndIdentify(SerializablePointer &pointer, Json::Value &data, Serializable *serializable);
	int idCounter;
	std::map<Serializable*, int> ids;
	Scene *deserializeScene(Json::Value &root);
	Serializer() = default;
	std::string SCENES_DIR = "Scenes";
	const std::string FORMAT = ".scene";
	const std::string OLD_FORMAT = ".oldscene";
	std::map<std::string, std::string> scenes;
};
