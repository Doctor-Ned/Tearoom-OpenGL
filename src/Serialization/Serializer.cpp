#include "Serializer.h"
#include <filesystem>
#include "Scene/GraphNode.h"

namespace fs = std::experimental::filesystem;

Serializer *Serializer::getInstance() {
	static Serializer* instance = nullptr;
	if (instance == nullptr) {
		instance = new Serializer();
	}
	return instance;
}

void Serializer::setup() {
	loadScenes();
}

void Serializer::saveScene(GraphNode* scene, const std::string& name) {
	idCounter = 0;
	ids.clear();
	SerializablePointer pointer(SGraphNode, scene);
	Json::Value root;
	serialize(pointer, root);
	std::string file = SCENES_DIR + "/" + name + FORMAT;
	if (fs::exists(file)) {
		std::string oldFile = SCENES_DIR + "/" + name + OLD_FORMAT;
		if (fs::exists(oldFile)) {
			fs::remove(oldFile);
		}
		fs::rename(file, oldFile);
	}
	Global::saveToFile(file, root);
	loadScenes();
}

GraphNode* Serializer::loadScene(const std::string& name) {
	std::string location;
	for (auto &pair : scenes) {
		if (name == pair.first) {
			location = pair.second;
			break;
		}
	}
	if (location.length() == 0) {
		return nullptr;
	}
	idCounter = 0;
	ids.clear();
	Json::Value root = Global::readJsonFile(location);
	return deserializeScene(root);
}

void* Serializer::getPointer(const int id) {
	for (auto &pair : ids) {
		if (pair.second == id) {
			return pair.first;
		}
	}
	return nullptr;
}

int Serializer::getId(void* pointer) {
	if (pointer == nullptr) {
		return -1;
	}
	for (auto &pair : ids) {
		if (pair.first == pointer) {
			return pair.second;
		}
	}
	return -1;
}


void Serializer::serialize(SerializablePointer ser, Json::Value& root) {
	if (ser.object == nullptr) {
		// if object is nullptr (this happens if we encounter a real nullptr in the tree
		// OR if the object has already been serialized and we just want to save the reference ID)
		root["id"] = -1;
		root["type"] = static_cast<int>(ser.type);
		return;
	}
	int id = getId(ser.object);
	if (id != -1) {
		// if the object has already been serialized, set the pointer to nullptr and just serialize the reference ID
		ser.object = nullptr;
		serialize(ser, root);
		return;
	}
	// if it gets here then it means this is the first time serializing given object pointer
	// assign the ID and apply the serialization
	ser.id = idCounter++;
	ids.emplace(ser.object, ser.id);
	//todo
}

GraphNode* Serializer::deserializeScene(Json::Value& root) {
	SerializablePointer pointer = deserialize(root);
	if (pointer.object == nullptr) {
		return nullptr;
	}
	return reinterpret_cast<GraphNode*>(pointer.object);
}

SerializablePointer Serializer::deserialize(Json::Value& root) {
	SerializablePointer pointer;
	pointer.id = root["id"].asInt();
	pointer.type = static_cast<SerializableType>(root["type"].asInt());
	if (pointer.id == -1) {   // entry is nullptr
		return pointer;
	}
	void *ptr = getPointer(pointer.id);
	if (ptr != nullptr) {     // entry is a reference to an already deserialized object pointer
		pointer.object = ptr;
		return pointer;
	}

	Json::Value data = root["object"];
	switch (pointer.type) {
		default:
			throw std::exception("Unsupported SerializableType encountered!");
		// remember to always emplace the pointer in the 'ids' map before recursively going down!
		case SGraphNode:
			GraphNode *node = new GraphNode();
			ids.emplace(node, pointer.id);
			//todo
			break;
	}
	return pointer;
}

void Serializer::loadScenes() {
	scenes.clear();
	for (auto &entry : fs::directory_iterator(SCENES_DIR)) {
		std::string filename = entry.path().filename().u8string();
		if (Global::endsWith(filename, FORMAT)) {
			scenes.emplace(filename.substr(0, filename.length() - FORMAT.length()), entry.path().u8string());
		}
	}
}
