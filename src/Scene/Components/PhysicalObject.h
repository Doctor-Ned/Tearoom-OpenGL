//
// Created by MB on 4/13/2019.
//
#ifndef TEAROOM_OPENGL_PHYSICALOBJECT_H
#define TEAROOM_OPENGL_PHYSICALOBJECT_H
#include "Component.h"
#include "Mesh/Mesh.h"
#include "Render/Renderable.h"
#include <queue>

template<typename T, typename Container = std::deque<T>>
class iterable_queue : public std::queue<T, Container> {
public:
	typedef typename Container::iterator iterator;
	typedef typename Container::const_iterator const_iterator;

	iterator begin() { return this->c.begin(); }
	iterator end() { return this->c.end(); }
	const_iterator begin() const { return this->c.begin(); }
	const_iterator end() const { return this->c.end(); }
};

class PhysicalObject: public Component, public Renderable {
public:
	bool isActive() override;
    PhysicalObject(GraphNode* _gameObject);
    virtual ~PhysicalObject();
    void update(float msec) override;
    void updateDrawData() override;
    void drawSelf(Shader* shader) override;
    ShaderType getShaderType() override;
	bool isOpaque() override;
    bool castRayDown();
	void pushTranslation(glm::vec3 translation);
	glm::vec3 direction = glm::vec3(0);
	float distance = 0.0f;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer *serializer) override;
	void deserialize(Json::Value &root, Serializer* serializer) override;
protected:
	iterable_queue<glm::vec3> lastKnownPosition;
	iterable_queue<glm::vec3> translations;
    bool isFalling = false;
    Mesh* mesh = nullptr;
	bool gravity = false;
	PhysicalObject(){}
	friend class Serializer;
};


#endif //TEAROOM_OPENGL_PHYSICALOBJECT_H
