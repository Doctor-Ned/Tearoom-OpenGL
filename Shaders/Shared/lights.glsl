struct DirLight {
	mat4 lightSpace;
	vec4 color;
	mat4 model;
	float near_plane;
	float far_plane;
	float strength;
	bool enabled;
	vec3 padding;
	float proj_size;
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
	float near_plane;
	float far_plane;
	float outerCutOff;
	bool enabled;
};

#define MAX_LIGHTS_OF_TYPE 4

layout (std140) uniform Lights {
	float initialAmbient;
	int dirLights;
	int spotLights;
	int pointLights;
	vec3 padding;
	float lightBleedingReduction;
	float minVariance;
	int pointShadowSamples;           // 20 gives a nice result but i could probably experiment with some other values.
	bool enableLights;
	bool enableShadowCasting;
	DirLight dirLight[MAX_LIGHTS_OF_TYPE];
	SpotLight spotLight[MAX_LIGHTS_OF_TYPE];
	PointLight pointLight[MAX_LIGHTS_OF_TYPE];
};