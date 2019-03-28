#ifndef UBOVIEWPROJECTION_H
#define UBOVIEWPROJECTION_H

#include "Ubo.h"

class UboViewProjection : public Ubo {
public:
	UboViewProjection(glm::mat4 view, glm::mat4 projection);
	void inject(glm::mat4 view, glm::mat4 projection);
};

#endif
