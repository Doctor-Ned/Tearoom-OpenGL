struct DirLight {
	mat4 lightSpace;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	mat4 model;
	int enabled;
	vec3 padding;
};

struct PointLight {
	float constant;
	float linear;
	float quadratic;
	float near_plane;
	int enabled;
	vec2 padding;
	float far_plane;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	mat4 model;
};

struct SpotLight {
	mat4 lightSpace;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	mat4 model;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	int enabled;
	vec2 padding;
	float outerCutOff;
};

#define MAX_LIGHTS_OF_TYPE 4

layout (std140) uniform Lights {
	float initialAmbient;
	int dirLights;
	int spotLights;
	int pointLights;
	DirLight dirLight[MAX_LIGHTS_OF_TYPE];
	SpotLight spotLight[MAX_LIGHTS_OF_TYPE];
	PointLight pointLight[MAX_LIGHTS_OF_TYPE];
};