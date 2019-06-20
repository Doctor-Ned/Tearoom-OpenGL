#ifndef COLUMN_MOVING_H
#define COLUMN_MOVING_H
#include "Scene/Components/Component.h"
#include "Scene/Components/LightComponents/Sun.h"

class ColumnMoving : public Component
{
protected:
	Sun* sun = nullptr;
	std::map<int, float> heightToHourMap;
	glm::vec3 lastPos;
	bool enableSound = false;
	void renderGui() override;
	float getFractionalPartOfHour(float hour);
	float getHeightForTime(float time);
public:
	ColumnMoving(GraphNode* gameObject, std::string name = "ColumnMoving");
	ColumnMoving() = default;
	virtual ~ColumnMoving();
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	void updateWorld() override;
	bool isComponentActive() override;
	void setComponentActive(bool active) override;
	void update(float msec) override;
	void setSun(Sun* sun);
	void addHeightOnSelectedHour(int hour, float height);
	void removeHeightOnSelectedHour(int hour);
};

#endif