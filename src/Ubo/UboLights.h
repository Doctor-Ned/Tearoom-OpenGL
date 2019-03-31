#ifndef UBOLIGHTS_H
#define UBOLIGHTS_H

#include "Ubo.h"

class UboLights : public Ubo {
public:
	UboLights(float ambient, int dirLights, int spotLights, int pointLights, DirLight *dirLight, SpotLight *spotLight, PointLight *pointLight);
	void inject(float ambient, int dirLights, int spotLights, int pointLights, DirLight *dirLight, SpotLight *spotLight, PointLight *pointLight);
};

#endif
