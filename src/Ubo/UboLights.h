#ifndef UBOLIGHTS_H
#define UBOLIGHTS_H

#include "Ubo.h"

struct PointLight;
struct SpotLight;
struct DirLight;

class UboLights : public Ubo {
public:
	UboLights(float ambient, int dirLights, int spotLights, int pointLights, bool enableLights, bool enableShadowCasting, float lightBleedingReduction, float minVariance, int pointShadowSamples, DirLight *dirLight, SpotLight *spotLight, PointLight *pointLight);
	void inject(float ambient, int dirLights, int spotLights, int pointLights, bool enableLights, bool enableShadowCasting, float lightBleedingReduction, float minVariance, int pointShadowSamples, DirLight *dirLight, SpotLight *spotLight, PointLight *pointLight);
};

#endif
