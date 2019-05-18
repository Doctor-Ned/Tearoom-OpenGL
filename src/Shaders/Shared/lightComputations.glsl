uniform int useLight;
uniform int castShadows;
uniform sampler2D dir_shadows[MAX_LIGHTS_OF_TYPE];
uniform sampler2D spot_shadows[MAX_LIGHTS_OF_TYPE];
uniform samplerCube point_shadows[MAX_LIGHTS_OF_TYPE];

const float PI = 3.14159265359;
#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38
#define DBL_MAX 1.7976931348623158e+308
#define DBL_MIN 2.2250738585072014e-308

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float distributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float geometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

vec3 calcDirLight(DirLight light, sampler2D tex, vec4 space, vec3 albedo, float roughness, float metallic, float ao, vec3 N, vec3 V) {
	float shadow = 1.0f;
	if (castShadows > 0 && enableShadowCasting) {
		vec3 projCoords = space.xyz / space.w;
		vec3 moments = texture(tex, projCoords.xy).rgb;
		float compare = projCoords.z;
		float p = step(compare, moments.x);
		float variance = max(moments.y - moments.x * moments.x, 0.00002);

		float d = compare - moments.x;
		float pMax = variance / (variance + d * d);

		shadow = min(max(p, pMax), 1.0f);

		if (compare > 1.0) {
			shadow = 1.0f;
		}
	}

	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, albedo, metallic);
	vec3 Lo = vec3(0.0f);
	mat4 directionWorld = light.model;
	directionWorld[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec3 L = normalize(vec3(directionWorld * -vec4(0.0f, 0.0f, -1.0f, 1.0f)));  // direction
	vec3 H = normalize(V + L);
	vec3 radiance = light.color.rgb * light.strength;

	float NDF = distributionGGX(N, H, roughness);
	float NdotV = max(dot(N, V), 0.0f);
	float NdotL = max(dot(N, L), 0.0f);
	float G = geometrySchlickGGX(NdotV, roughness);
	vec3 F = fresnelSchlickRoughness(max(dot(H, V), 0.0f), F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0f) - kS;
	kD *= 1.0f - metallic;

	vec3 numerator = NDF * G*F;
	float denominator = 4.0f * NdotV * NdotL;
	vec3 specular = numerator / max(denominator, 0.001f);

	Lo += (kD * albedo / PI + specular) * radiance * NdotL;

	//vec3 color = initialAmbient * albedo * ao;
	vec3 color = initialAmbient * albedo;
	return color + Lo * shadow * ao;
}

vec3 calcSpotLight(SpotLight light, sampler2D tex, vec4 space, vec3 albedo, float roughness, float metallic, float ao, vec3 N, vec3 V) {
	float shadow = 0.0;
	if (castShadows > 0 && enableShadowCasting) {
		vec3 projCoords = space.xyz / space.w;
		vec3 moments = texture(tex, projCoords.xy).rgb;
		float compare = projCoords.z;
		float p = step(compare, moments.x);
		float variance = max(moments.y - moments.x * moments.x, 0.00002);

		float d = compare - moments.x;
		float pMax = variance / (variance + d * d);

		shadow = min(max(p, pMax), 1.0f);

		if (compare > 1.0) {
			shadow = 1.0;
		}
	}
	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, albedo, metallic);
	vec3 Lo = vec3(0.0f);
	vec3 position = vec3(light.model[3]);
	vec3 L = normalize(position - fs_in.pos); //direction
	vec3 H = normalize(V + L);

	mat4 directionWorld = light.model;
	directionWorld[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec3 spotDirection = normalize(vec3(directionWorld * -vec4(0.0f, 0.0f, -1.0f, 1.0f)));

	float dist = length(position - fs_in.pos);
	float attenuation;
	if (light.constant < 0.001f && light.linear < 0.001f && light.quadratic < 0.001f) {
		attenuation = FLT_MAX;
	} else {
		attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
	}

	vec3 radiance = light.color.rgb * attenuation;

	float NDF = distributionGGX(N, H, roughness);
	float NdotV = max(dot(N, V), 0.0f);
	float NdotL = max(dot(N, L), 0.0f);
	float G = geometrySchlickGGX(NdotV, roughness);
	vec3 F = fresnelSchlickRoughness(max(dot(H, V), 0.0), F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0f) - kS;
	kD *= 1.0 - metallic;

	vec3 numerator = NDF * G*F;
	float denominator = 4.0f * NdotV * NdotL;
	vec3 specular = numerator / max(denominator, 0.001f);

	Lo += (kD * albedo / PI + specular) * radiance * NdotL;

	//vec3 color = initialAmbient * albedo * ao;
	vec3 color = initialAmbient * albedo;

	float cutOff = cos(light.cutOff);
	float outerCutOff = cos(light.outerCutOff);

	float theta = dot(L, spotDirection);
	float epsilon = cutOff - outerCutOff;
	float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

	return color + Lo * intensity * shadow * ao;
}

vec3 gridSamplingDisk[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
	);

vec3 calcPointLight(PointLight light, samplerCube tex, vec3 albedo, float roughness, float metallic, float ao, vec3 N, vec3 V) {
	float shadow = 0.0;
	if (castShadows > 0 && enableShadowCasting) {
		vec3 fragToLight = fs_in.pos - vec3(light.model[3]);
		float currentDepth = length(fragToLight);
		float bias = 0.15;
		float viewDistance = length(fs_in.viewPosition - fs_in.pos);
		float diskRadius = (1.0 + (viewDistance / light.far_plane)) / 25.0;
		for (int i = 0; i < pointShadowSamples; ++i) {
			float closestDepth = texture(tex, fragToLight + gridSamplingDisk[i] * diskRadius).r;
			closestDepth *= light.far_plane;
			if (currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		shadow /= float(pointShadowSamples);
	}

	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, albedo, metallic);
	vec3 Lo = vec3(0.0f);
	vec3 position = vec3(light.model[3]);
	vec3 L = normalize(position - fs_in.pos); //direction
	vec3 H = normalize(V + L);

	mat4 directionWorld = light.model;
	directionWorld[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec3 spotDirection = normalize(vec3(directionWorld * -vec4(0.0f, 0.0f, -1.0f, 1.0f)));

	float dist = length(position - fs_in.pos);
	float attenuation;
	if (light.constant < 0.001f && light.linear < 0.001f && light.quadratic < 0.001f) {
		attenuation = FLT_MAX;
	} else {
		attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
	}

	vec3 radiance = light.color.rgb * attenuation;

	float NDF = distributionGGX(N, H, roughness);
	float NdotV = max(dot(N, V), 0.0f);
	float NdotL = max(dot(N, L), 0.0f);
	float G = geometrySchlickGGX(NdotV, roughness);
	vec3 F = fresnelSchlickRoughness(max(dot(H, V), 0.0), F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0f) - kS;
	kD *= 1.0 - metallic;

	vec3 numerator = NDF * G*F;
	float denominator = 4.0f * NdotV * NdotL;
	vec3 specular = numerator / max(denominator, 0.001f);

	Lo += (kD * albedo / PI + specular) * radiance * NdotL;

	//vec3 color = initialAmbient * albedo * ao;
	vec3 color = initialAmbient * albedo;
	return color + Lo * (1.0f - shadow) * ao;
}