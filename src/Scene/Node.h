#pragma once
#include "GraphNode.h"

class Node {
public:
	static GraphNode *createBox(glm::vec3 dimensions, glm::vec4 color, GraphNode *parent = nullptr);
	static GraphNode *createBox(glm::vec3 dimensions, std::string texture, GraphNode *parent = nullptr);
	static GraphNode *createBox(glm::vec3 dimensions, bool reflective, GraphNode *parent = nullptr);
	static GraphNode *createSphere(float radius, int precision, glm::vec4 color, GraphNode *parent = nullptr);
	static GraphNode *createSphere(float radius, int precision, std::string texture, GraphNode *parent = nullptr);
	static GraphNode *createSphere(float radius, int precision, bool reflective, GraphNode *parent = nullptr);
};
