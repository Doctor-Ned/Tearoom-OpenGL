//
// Created by MB on 4/20/2019.
//

#ifndef TEAROOM_COLLECTABLEOBJECT_H
#define TEAROOM_COLLECTABLEOBJECT_H
#include "Component.h"
#include "Render/Renderable.h"
#include "Mesh/Mesh.h"

class CollectableObject: public Component {

public:
    virtual ~CollectableObject();
    CollectableObject(GraphNode* _gameObject);
    void takeObject();
    void leaveObject();
    void update(float msec) override;

protected:
    bool isTaken = false;
    bool isHitByRay = false;
    bool fKeyState = false;
};


#endif //TEAROOM_COLLECTABLEOBJECT_H
