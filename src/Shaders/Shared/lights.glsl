struct DirLight {
	mat4 lightSpace;
	vec4 color;
	mat4 model;
	vec2 padding;
	float strength;
	bool enabled;
};

struct PointLight {
	float constant;
	float linear;
	float quadratic;
	float near_plane;
	vec2 padding;
	float far_plane;
	bool enabled;
	vec4 color;
	mat4 model;
};

struct SpotLight {
	mat4 lightSpace;
	vec4 color;
	mat4 model;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	vec2 padding;
	float outerCutOff;
	bool enabled;
};

#define MAX_LIGHTS_OF_TYPE 4

layout (std140) uniform Lights {
	float initialAmbient;
	int dirLights;
	int spotLights;
	int pointLights;
	int spotDirShadowTexelResolution; // this should be a power of 3. 3 is a nice base value btw. 1 should also be acceptable.
	int pointShadowSamples;           // 20 gives a nice result but i could probably experiment with some other values.
	bool enableLights;
	bool enableShadowCasting;
	DirLight dirLight[MAX_LIGHTS_OF_TYPE];
	SpotLight spotLight[MAX_LIGHTS_OF_TYPE];
	PointLight pointLight[MAX_LIGHTS_OF_TYPE];
};