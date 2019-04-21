#include "Serializer.h"
#include <filesystem>
#include "Scene/GraphNode.h"
#include "Mesh/MeshBox.h"
#include "Mesh/MeshColorBox.h"
#include "Mesh/MeshColorCone.h"
#include "Mesh/MeshColorCylinder.h"
#include "Mesh/MeshColorPlane.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/MeshColorTorus.h"
#include "Mesh/MeshTorus.h"
#include "Mesh/ModelInstanced.h"
#include "Mesh/MeshSphere.h"
#include "Mesh/MeshRefSphere.h"
#include "Mesh/MeshRefBox.h"
#include "Mesh/MeshPlane.h"
#include "Mesh/MeshCylinder.h"
#include "Mesh/MeshCone.h"
#include "Mesh/Model.h"

namespace fs = std::experimental::filesystem;

Serializer *Serializer::getInstance() {
	static Serializer* instance = nullptr;
	if (instance == nullptr) {
		instance = new Serializer();
	}
	return instance;
}

void Serializer::setup() {
	if(!fs::exists(SCENES_DIR)) {
		fs::create_directory(SCENES_DIR);
	}
	loadScenes();
}

void Serializer::saveScene(GraphNode* scene, const std::string& name) {
	idCounter = 0;
	ids.clear();
	SerializablePointer pointer(SGraphNode, scene);
	Json::Value root = serialize(pointer);
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

Serializable* Serializer::getPointer(const int id) {
	for (auto &pair : ids) {
		if (pair.second == id) {
			return pair.first;
		}
	}
	return nullptr;
}

int Serializer::getId(Serializable* pointer) {
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

Json::Value Serializer::serialize(Serializable* ser) {
	return serialize(SerializablePointer(ser));
}


Json::Value Serializer::serialize(SerializablePointer ser) {
	Json::Value root;
	if (ser.object == nullptr) {
		// if object is nullptr (this happens if we encounter a real nullptr in the tree
		// OR if the object has already been serialized and we just want to save the reference ID)
		root["id"] = -1;
		root["type"] = static_cast<int>(SNone);
		return root;
	}
	int id = getId(ser.object);
	if (id != -1) {
		// if the object has already been serialized, set the pointer to nullptr and just serialize the reference ID
		ser.object = nullptr;
		return serialize(ser);
	}
	// if it gets here then it means this is the first time serializing given object pointer
	// assign the ID and apply the serialization
	ser.id = idCounter++;
	ids.emplace(ser.object, ser.id);
	root["id"] = ser.id;
	root["type"] = ser.object->getSerializableType();
	root["object"] = ser.object->serialize(this);
	return root;
}

GraphNode* Serializer::deserializeScene(Json::Value& root) {
	SerializablePointer pointer = deserialize(root);
	if (pointer.object == nullptr) {
		return nullptr;
	}
	return dynamic_cast<GraphNode*>(pointer.object);
}

SerializablePointer Serializer::deserialize(Json::Value& root) {
	SerializablePointer pointer;
	pointer.id = root["id"].asInt();
	if (pointer.id == -1) {   // entry is nullptr
		return pointer;
	}
	Serializable *ptr = getPointer(pointer.id);
	if (ptr != nullptr) {     // entry is a reference to an already deserialized object pointer
		pointer.object = ptr;
		return pointer;
	}

	SerializableType type = static_cast<SerializableType>(root["type"].asInt());

	Json::Value data = root["object"];
	switch (type) {
		default:
			throw std::exception("Unsupported SerializableType encountered!");
		// remember to always emplace the pointer in the 'ids' map before calling serialize (which can recursively go down the tree)!
		case SGraphNode:
			deserializeAndIdentify(pointer, data, new GraphNode());
			break;
		case SMeshBox:
			deserializeAndIdentify(pointer, data, new MeshBox());
			break;
		case SMeshColorBox:
			deserializeAndIdentify(pointer, data, new MeshColorBox());
			break;
		case SMeshColorCone:
			deserializeAndIdentify(pointer, data, new MeshColorCone());
			break;
		case SMeshColorCylinder:
			deserializeAndIdentify(pointer, data, new MeshColorCylinder());
			break;
		case SMeshColorPlane:
			deserializeAndIdentify(pointer, data, new MeshColorPlane());
			break;
		case SMeshColorSphere:
			deserializeAndIdentify(pointer, data, new MeshColorSphere());
			break;
		case SMeshColorTorus:
			deserializeAndIdentify(pointer, data, new MeshColorTorus());
			break;
		case SMeshCone:
			deserializeAndIdentify(pointer, data, new MeshCone());
			break;
		case SMeshCylinder:
			deserializeAndIdentify(pointer, data, new MeshCylinder());
			break;
		case SMeshPlane:
			deserializeAndIdentify(pointer, data, new MeshPlane());
			break;
		case SMeshRefBox:
			deserializeAndIdentify(pointer, data, new MeshRefBox());
			break;
		case SMeshRefSphere:
			deserializeAndIdentify(pointer, data, new MeshRefSphere());
			break;
		case SMeshSphere:
			deserializeAndIdentify(pointer, data, new MeshSphere());
			break;
		case SModel:
			deserializeAndIdentify(pointer, data, new Model());
			break;
		case SModelInstanced:
			deserializeAndIdentify(pointer, data, new ModelInstanced());
			break;
		case SMeshTorus:
			deserializeAndIdentify(pointer, data, new MeshTorus());
			break;
	}
	return pointer;
}

void Serializer::deserializeAndIdentify(SerializablePointer& pointer, Json::Value &data, Serializable* serializable) {
	pointer.object = serializable;
	ids.emplace(serializable, pointer.id);
	serializable->deserialize(data, this);
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
