uniform int useLight;
uniform int castShadows;
uniform float shininess;
uniform sampler2D dir_shadows[MAX_LIGHTS_OF_TYPE];
uniform sampler2D spot_shadows[MAX_LIGHTS_OF_TYPE];
uniform samplerCube point_shadows[MAX_LIGHTS_OF_TYPE];

#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38
#define DBL_MAX 1.7976931348623158e+308
#define DBL_MIN 2.2250738585072014e-308

vec3 calcDirLight(DirLight light, sampler2D tex, vec4 space, vec3 diffuse, vec3 specular, vec3 viewDir) {
    float shadow = 0.0;
	if(castShadows > 0) {
		vec3 projCoords = space.xyz / space.w;
		projCoords = projCoords * 0.5 + 0.5;
		float closestDepth = texture(tex, projCoords.xy).r; 
		float currentDepth = projCoords.z;
		vec3 lightDir = normalize(vec3(light.model[3]) - fs_in.pos);
		float bias = max(0.05 * (1.0 - dot(fs_in.normal, lightDir)), 0.005);
		if (spotDirShadowTexelResolution <= 1) {
			shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
		} else {
			vec2 texelSize = 1.0 / textureSize(tex, 0);
			int offset = (spotDirShadowTexelResolution - 1) / 2;
			for(int x = -offset; x <= offset; ++x)
			{
				for(int y = -offset; y <= offset; ++y)
				{
					float pcfDepth = texture(tex, projCoords.xy + vec2(x, y) * texelSize).r; 
					shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
				}    
			}
			shadow /= float(spotDirShadowTexelResolution * spotDirShadowTexelResolution);
		}
    
		if(projCoords.z > 1.0) {
			shadow = 0.0;
		}
	}

	//mat4 directionWorld = light.model;
	//directionWorld[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec3 direction = normalize(vec3(light.model * -vec4(0.0f, 0.0f, -1.0f, 1.0f)));
    float diff = max(dot(direction, fs_in.normal), 0.0);

	vec3 reflectDir = reflect(-direction, fs_in.normal);
	vec3 halfwayDir = normalize(direction + viewDir);
    float spec = pow(max(dot(fs_in.normal, halfwayDir), 0.0), shininess);

	return (vec3(light.ambient) * diffuse) + ((vec3(light.diffuse) * diff * diffuse) + (vec3(light.specular) * spec * specular)) * (1 - vec3(shadow));
}

vec3 calcSpotLight(SpotLight light, sampler2D tex, vec4 space, vec3 diffuse, vec3 specular, vec3 viewDir) {
	float shadow = 0.0;
	if(castShadows > 0) {
		vec3 projCoords = space.xyz / space.w;
		projCoords = projCoords * 0.5 + 0.5;
		float closestDepth = texture(tex, projCoords.xy).r; 
		float currentDepth = projCoords.z;
		vec3 lightDir = normalize(vec3(light.model[3]) - fs_in.pos);
		float bias = max(0.05 * (1.0 - dot(fs_in.normal, lightDir)), 0.005);

		if (spotDirShadowTexelResolution <= 1) {
			shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
		} else {
			vec2 texelSize = 1.0 / textureSize(tex, 0);
			int offset = (spotDirShadowTexelResolution - 1) / 2;
			for (int x = -offset; x <= offset; ++x) {
				for (int y = -offset; y <= offset; ++y) {
					float pcfDepth = texture(tex, projCoords.xy + vec2(x, y) * texelSize).r;
					shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
				}
			}
			shadow /= float(spotDirShadowTexelResolution * spotDirShadowTexelResolution);
		}
    
		if(projCoords.z > 1.0) {
			shadow = 0.0;
		}
	}
	vec3 position = vec3(light.model[3]);
	vec3 direction = normalize(position - fs_in.pos);
	//mat4 directionWorld = light.model;
	//directionWorld[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec3 spotDirection = normalize(vec3(light.model * vec4(0.0f, 0.0f, -1.0f, 1.0f)));
	float diff = max(dot(direction, fs_in.normal), 0.0);

	float cutOff = cos(light.cutOff);
	float outerCutOff = cos(light.outerCutOff);
	
	vec3 reflectDir = reflect(-direction, fs_in.normal);
	vec3 halfwayDir = normalize(direction + viewDir);
    float spec = pow(max(dot(fs_in.normal, halfwayDir), 0.0), shininess);
	
	float dist = length(position - fs_in.pos);
	float attenuation;
	if (light.constant < 0.001f && light.linear < 0.001f && light.quadratic < 0.001f) {
		attenuation = FLT_MAX;
	} else {
		attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
	}
	
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
	float shadow = 0.0;
	if(castShadows > 0) {
		vec3 fragToLight = fs_in.pos - vec3(light.model[3]);
		float currentDepth = length(fragToLight);
		float bias = 0.15;
		float viewDistance = length(fs_in.viewPosition - fs_in.pos);
		float diskRadius = (1.0 + (viewDistance / light.far_plane)) / 25.0;
		for(int i = 0; i < pointShadowSamples; ++i)
		{
			float closestDepth = texture(tex, fragToLight + gridSamplingDisk[i] * diskRadius).r;
			closestDepth *= light.far_plane;
			if(currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		shadow /= float(pointShadowSamples);
	}
	vec3 position = vec3(light.model[3]);
	vec3 direction = normalize(position - fs_in.pos);
	float diff = max(dot(direction, fs_in.normal), 0.0);
	
	vec3 reflectDir = reflect(-direction, fs_in.normal);
	vec3 halfwayDir = normalize(direction + viewDir);
    float spec = pow(max(dot(fs_in.normal, halfwayDir), 0.0), shininess);

	float dist = length(position - fs_in.pos);

	float attenuation;
	if (light.constant < 0.001f && light.linear < 0.001f && light.quadratic < 0.001f) {
		attenuation = FLT_MAX;
	} else {
		attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
	}

	return (((vec3(light.ambient) * diffuse) + (vec3(light.diffuse) * diff * diffuse) + (vec3(light.specular) * spec * specular)) * vec3(1-shadow) * attenuation);
}