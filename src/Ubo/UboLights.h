#ifndef UBOLIGHTS_H
#define UBOLIGHTS_H

#include "Ubo.h"

struct PointLight;
struct SpotLight;
struct DirLight;

class UboLights : public Ubo {
public:
	UboLights(float ambient, int dirLights, int spotLights, int pointLights, bool enableLights, bool enableShadowCasting, int spotDirShadowTexelResolution, int pointShadowSamples, DirLight *dirLight, SpotLight *spotLight, PointLight *pointLight, float *dirCascadeSplits);
	void inject(float ambient, int dirLights, int spotLights, int pointLights, bool enableLights, bool enableShadowCasting, int spotDirShadowTexelResolution, int pointShadowSamples, DirLight *dirLight, SpotLight *spotLight, PointLight *pointLight, float *dirCascadeSplits);
};

#endif
