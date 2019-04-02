#ifndef COMPONENT_H
#define COMPONENT_H
#include <string>

class GraphNode;
class Component abstract
{
protected:
	GraphNode* gameObject;
	Component(GraphNode* _gameObject) : gameObject(_gameObject) { }
public:
	virtual std::string toString() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual ~Component() { }
};
#endif