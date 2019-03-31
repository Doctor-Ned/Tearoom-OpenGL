#version 330 core

struct DirLight {
	mat4 lightSpace;
	vec4 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	mat4 model;
};

struct PointLight {
	vec4 position;
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
	vec4 position;
	vec4 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	mat4 model;
	vec3 padding;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};

layout (std140) uniform TextureColor {
	vec4 tcolor;
	bool disableTexture;
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

in VS_OUT {
	vec3 pos;
	vec2 texCoords;
	vec3 normal;
	vec3 viewPosition;
	vec4 fragDirSpaces[MAX_LIGHTS_OF_TYPE];
	vec4 fragSpotSpaces[MAX_LIGHTS_OF_TYPE];
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D dir_shadows[MAX_LIGHTS_OF_TYPE];
uniform sampler2D spot_shadows[MAX_LIGHTS_OF_TYPE];
uniform samplerCube point_shadows[MAX_LIGHTS_OF_TYPE];
uniform float shininess;
uniform mat4 model;
uniform int useSpecularMap;
uniform int useLight;

out vec4 outColor;

vec3 calcDirLight(DirLight light, sampler2D tex, vec4 space, vec3 diffuse, vec3 specular, vec3 viewDir) {
	vec3 projCoords = space.xyz / space.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(tex, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 lightDir = normalize(vec3(light.model[3]) - fs_in.pos);
    float bias = max(0.05 * (1.0 - dot(fs_in.normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(tex, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(tex, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;

	vec3 direction = normalize(vec3(light.model * (-light.direction)));
    float diff = max(dot(direction, fs_in.normal), 0.0);

	vec3 reflectDir = reflect(-direction, fs_in.normal);
	vec3 halfwayDir = normalize(direction + viewDir);
    float spec = pow(max(dot(fs_in.normal, halfwayDir), 0.0), shininess);

	return (vec3(light.ambient) * diffuse) + ((vec3(light.diffuse) * diff * diffuse) + (vec3(light.specular) * spec * specular)) * (1 - vec3(shadow));
}

vec3 calcSpotLight(SpotLight light, sampler2D tex, vec4 space, vec3 diffuse, vec3 specular, vec3 viewDir) {
	vec3 projCoords = space.xyz / space.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(tex, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 lightDir = normalize(vec3(light.model * light.position) - fs_in.pos);
    float bias = max(0.05 * (1.0 - dot(fs_in.normal, lightDir)), 0.005);
    
	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(tex, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(tex, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;

	vec3 position = vec3(light.model * light.position);
	vec3 direction = normalize(position - fs_in.pos);
	vec3 spotDirection = normalize(vec3(light.model * vec4(vec3(-light.direction), 0.0f)));
	float diff = max(dot(direction, fs_in.normal), 0.0);

	float cutOff = cos(light.cutOff);
	float outerCutOff = cos(light.outerCutOff);
	
	vec3 reflectDir = reflect(-direction, fs_in.normal);
	vec3 halfwayDir = normalize(direction + viewDir);
    float spec = pow(max(dot(fs_in.normal, halfwayDir), 0.0), shininess);
	
	float dist = length(position - fs_in.pos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
	
	float theta = dot(direction, spotDirection);
	float epsilon = cutOff - outerCutOff;
	float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

	vec3 amb = vec3(light.ambient) * diffuse;
	vec3 dif = vec3(light.diffuse) * diff * diffuse * intensity;
	vec3 spe = vec3(light.specular) * spec * specular * intensity;

	return ((amb + (dif + spe) * (1 - vec3(shadow)) * attenuation));
}

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

vec3 calcPointLight(PointLight light, samplerCube tex, vec3 diffuse, vec3 specular, vec3 viewDir) {
    vec3 fragToLight = fs_in.pos - vec3(light.model * light.position);
    float currentDepth = length(fragToLight);
	float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(fs_in.viewPosition - fs_in.pos);
    float diskRadius = (1.0 + (viewDistance / light.far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(tex, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= light.far_plane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

	vec3 position = vec3(light.model * vec4(0.0f, 0.0f, 0.0f, 1.0f));
	vec3 direction = normalize(position - fs_in.pos);
	float diff = max(dot(direction, fs_in.normal), 0.0);
	
	vec3 reflectDir = reflect(-direction, fs_in.normal);
	vec3 halfwayDir = normalize(direction + viewDir);
    float spec = pow(max(dot(fs_in.normal, halfwayDir), 0.0), shininess);

	float dist = length(position - fs_in.pos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	return (((vec3(light.ambient) * diffuse) + (vec3(light.diffuse) * diff * diffuse) + (vec3(light.specular) * spec * specular)) * vec3(1-shadow) * attenuation);
}

void main() {
	vec3 diffuse = tcolor.rgb;
	if(!disableTexture) diffuse *= texture(texture_diffuse1, fs_in.texCoords).rgb;
    vec3 ambient = initialAmbient * diffuse;
	if(useLight == 0) {
		outColor = vec4(diffuse, 1.0f);
	} else {
		vec3 specular = useSpecularMap > 0 ? texture(texture_specular1, fs_in.texCoords).rgb : vec3(0.5f);
		vec3 viewDir = normalize(fs_in.viewPosition - fs_in.pos);

		vec3 color = ambient;

		for(int i=0;i<dirLights;i++) {
			color += calcDirLight(dirLight[i], dir_shadows[i], fs_in.fragDirSpaces[i], diffuse, specular, viewDir);
		}
		for(int i=0;i<spotLights;i++) {
			color += calcSpotLight(spotLight[i], spot_shadows[i], fs_in.fragSpotSpaces[i], diffuse, specular, viewDir);
		}
		for(int i=0;i<pointLights;i++) {
			color += calcPointLight(pointLight[i], point_shadows[i], diffuse, specular, viewDir);
		}

		outColor = vec4(color, 1.0f);
	}
}