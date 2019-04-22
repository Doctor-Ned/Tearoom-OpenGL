#ifndef UBOLIGHTS_H
#define UBOLIGHTS_H

#include "Ubo.h"

struct PointLight;
struct SpotLight;
struct DirLight;

class UboLights : public Ubo {
public:
	UboLights(float ambient, int dirLights, int spotLights, int pointLights, int spotDirShadowTexelResolution, int pointShadowSamples, DirLight *dirLight, SpotLight *spotLight, PointLight *pointLight);
	void inject(float ambient, int dirLights, int spotLights, int pointLights, int spotDirShadowTexelResolution, int pointShadowSamples, DirLight *dirLight, SpotLight *spotLight, PointLight *pointLight);
};

#endif
