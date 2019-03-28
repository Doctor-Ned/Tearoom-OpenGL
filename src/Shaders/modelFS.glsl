#version 330 core

struct DirLight {
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
	float padding;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	mat4 model;
};

struct SpotLight {
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

layout (std140) uniform Lights {
	DirLight dirLight;
	SpotLight spotLight;
	PointLight pointLight;
};

in VS_OUT {
	vec3 pos;
	vec2 texCoords;
	vec3 normal;
	vec3 viewPosition;
	vec4 fragDirLightSpace;
	vec4 fragSpotLightSpace;
	vec4 fragPointLightSpace;
} fs_in;

uniform float near_plane;
uniform float far_plane;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D dir_shadows;
uniform sampler2D spot_shadows;
uniform samplerCube point_shadows;
uniform float shininess;
uniform mat4 model;
uniform int useSpecularMap;
uniform int useLight;

out vec4 outColor;

vec3 calcDirLight(DirLight light, vec3 diffuse, vec3 specular, vec3 viewDir) {
	vec3 projCoords = fs_in.fragDirLightSpace.xyz / fs_in.fragDirLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(dir_shadows, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 lightDir = normalize(vec3(light.model[3]) - fs_in.pos);
    float bias = max(0.05 * (1.0 - dot(fs_in.normal, lightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;

	vec3 direction = normalize(vec3(light.model * (-light.direction)));
    float diff = max(dot(direction, fs_in.normal), 0.0);

	vec3 reflectDir = reflect(-direction, fs_in.normal);
	vec3 halfwayDir = normalize(direction + viewDir);
    float spec = pow(max(dot(fs_in.normal, halfwayDir), 0.0), shininess);

	return (vec3(light.ambient) * diffuse) + ((vec3(light.diffuse) * diff * diffuse) + (vec3(light.specular) * spec * specular)) * (1 - vec3(shadow));
	//return ((vec3(light.ambient) * diffuse) + (vec3(light.diffuse) * diff * diffuse) + (vec3(light.specular) * spec * specular));
}

vec3 calcPointLight(PointLight light, vec3 diffuse, vec3 specular, vec3 viewDir) {
    vec3 fragToLight = fs_in.pos - vec3(light.model * light.position);
    float closestDepth = texture(point_shadows, fragToLight).r;
    closestDepth *= far_plane;
	closestDepth += near_plane;
    float currentDepth = length(fragToLight);
    float bias = 0.05; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

	vec3 position = vec3(light.model * light.position);
	vec3 direction = normalize(position - fs_in.pos);
	float diff = max(dot(direction, fs_in.normal), 0.0);
	
	vec3 reflectDir = reflect(-direction, fs_in.normal);
	vec3 halfwayDir = normalize(direction + viewDir);
    float spec = pow(max(dot(fs_in.normal, halfwayDir), 0.0), shininess);

	float dist = length(position - fs_in.pos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	return (((vec3(light.ambient) * diffuse) + (vec3(light.diffuse) * diff * diffuse) + (vec3(light.specular) * spec * specular)) * vec3(1-shadow) * attenuation);
}

vec3 calcSpotLight(SpotLight light, vec3 diffuse, vec3 specular, vec3 viewDir) {
	vec3 projCoords = fs_in.fragSpotLightSpace.xyz / fs_in.fragSpotLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(spot_shadows, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 lightDir = normalize(vec3(light.model * light.position) - fs_in.pos);
    float bias = max(0.05 * (1.0 - dot(fs_in.normal, lightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;

	vec3 position = vec3(light.model * vec4(0.0f, 0.0f, 0.0f, 1.0f));
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
	//return ((amb + dif + spe) * attenuation);
}

void main() {
	vec3 diffuse = tcolor.rgb;
	if(!disableTexture) diffuse *= texture(texture_diffuse1, fs_in.texCoords).rgb;
    vec3 ambient = 0.05 * diffuse;
	if(useLight == 0) {
		outColor = vec4(diffuse, 1.0f);
	} else {
		vec3 specular = useSpecularMap > 0 ? texture(texture_specular1, fs_in.texCoords).rgb : vec3(0.5f);
		vec3 viewDir = normalize(fs_in.viewPosition - fs_in.pos);

		vec3 color = ambient;

		color += calcDirLight(dirLight, diffuse, specular, viewDir);
		color += calcSpotLight(spotLight, diffuse, specular, viewDir);
		color += calcPointLight(pointLight, diffuse, specular, viewDir);

		outColor = vec4(color, 1.0f);
	}
}