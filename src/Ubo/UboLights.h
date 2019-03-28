#ifndef UBOLIGHTS_H
#define UBOLIGHTS_H

#include "Ubo.h"

class UboLights : public Ubo {
public:
	UboLights(DirLight *dirLight, SpotLight *spotLight, PointLight *pointLight);
	void inject(DirLight *dirLight, SpotLight *spotLight, PointLight *pointLight);
};

#endif
