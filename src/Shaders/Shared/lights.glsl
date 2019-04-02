struct DirLight {
	mat4 lightSpace;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	mat4 model;
};

struct PointLight {
	float constant;
	float linear;
	float quadratic;
	float near_plane;
	float far_plane;
	vec3 padding;
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
	float outerCutOff;
	vec3 padding;
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