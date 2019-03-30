#ifndef UBOLIGHTS_H
#define UBOLIGHTS_H

#include "Ubo.h"

class UboLights : public Ubo {
public:
	UboLights(float ambient, DirLight *dirLight, SpotLight *spotLight, PointLight *pointLight);
	void inject(float ambient, DirLight *dirLight, SpotLight *spotLight, PointLight *pointLight);
};

#endif
