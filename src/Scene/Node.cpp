#include "Node.h"
#include "Mesh/MeshColorBox.h"
#include "Mesh/MeshBox.h"
#include "Mesh/MeshRefBox.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/MeshSphere.h"
#include "Mesh/MeshRefSphere.h"

GraphNode* Node::createBox(glm::vec3 dimensions, glm::vec4 color, GraphNode* parent) {
	return new GraphNode(new MeshColorBox(dimensions, color), parent);
}

GraphNode * Node::createBox(glm::vec3 dimensions, std::string texture, GraphNode * parent) {
	return new GraphNode(new MeshBox(dimensions, texture), parent);
}

GraphNode * Node::createBox(glm::vec3 dimensions, bool reflective, GraphNode * parent) {
	return new GraphNode(new MeshRefBox(reflective, dimensions), parent);
}

GraphNode * Node::createSphere(float radius, int precision, glm::vec4 color, GraphNode * parent) {
	return new GraphNode(new MeshColorSphere(radius, precision, color), parent);
}

GraphNode * Node::createSphere(float radius, int precision, std::string texture, GraphNode * parent) {
	return new GraphNode(new MeshSphere(radius, precision, texture), parent);
}

GraphNode* Node::createSphere(float radius, int precision, bool reflective, GraphNode* parent) {
	return new GraphNode(new MeshRefSphere(reflective, radius, precision), parent);
}
