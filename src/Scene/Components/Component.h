#ifndef COMPONENT_H
#define COMPONENT_H

class GraphNode;
class Component abstract
{
protected:
	GraphNode* gameObject;
	Component(GraphNode* _gameObject) : gameObject(_gameObject)
	{
		
	}
public:
	virtual void update(float msec) = 0;
	virtual ~Component() { }
};
#endif