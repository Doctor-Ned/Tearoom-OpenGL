#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

layout (std140) uniform TextureColor {
	vec4 tcolor;
	bool disableTexture;
};

//%lights.glsl%

in VS_OUT {
	vec3 pos;
	vec2 texCoords;
	vec3 normal;
	vec3 viewPosition;
	vec4 fragDirSpaces[MAX_LIGHTS_OF_TYPE];
	vec4 fragSpotSpaces[MAX_LIGHTS_OF_TYPE];
	mat3 TBN;
	vec3 TanViewPos;
	vec3 TanFragPos;
} fs_in;

uniform sampler2D default_texture;

uniform sampler2D textures[7];
// ao, albedo, emissive, metallic, normal, roughness, height
//  0,      1,        2,        3,      4,         5,      6
uniform bool available[7];
uniform mat4 model;
uniform float opacity;
uniform float emissiveFactor;
uniform float depthScale;
//%lightComputations.glsl%
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) {
	/*float height = texture(textures[6], texCoords).r;
	vec2 p = viewDir.xy * (height * 0.1f);
	return texCoords - p;*/

	float heightScale = depthScale;
	// number of depth layers
	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy / viewDir.z * heightScale;
	vec2 deltaTexCoords = P / numLayers;

	vec2  currentTexCoords = texCoords;
	float currentDepthMapValue = 1.0f - texture(textures[6], currentTexCoords).r;

	while (currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = 1.0f - texture(textures[6], currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}

	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = (1.0f - texture(textures[6], prevTexCoords).r) - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}

void main() {
	vec2 texCoords = fs_in.texCoords;
	vec3 V = normalize(fs_in.viewPosition - fs_in.pos);
	if (available[6]) {
		//vec3 viewDir = normalize(fs_in.TanViewPos - fs_in.TanFragPos);
		vec3 viewDir = normalize(fs_in.TanViewPos - fs_in.TanFragPos);
		texCoords = ParallaxMapping(fs_in.texCoords, viewDir);
		V = normalize(fs_in.TanViewPos - fs_in.TanFragPos);
	}
	vec4 albedo = texture(available[1] ? textures[1] : default_texture, texCoords);
	vec3 albedoRGB = albedo.rgb;
	float roughness = available[5] ? texture(textures[5], texCoords).r : 1.0f;
	float metallic = available[3] ? texture(textures[3], texCoords).r : 0.0f;
	float ao = available[0] ? texture(textures[0], texCoords).r : 1.0f;
	//vec3 emissive = available[2] ? texture(textures[2], texCoords).rgb : vec3(0.0f, 0.0f, 0.0f);
	vec3 emissive = (available[2] ? texture(textures[2], texCoords).rgb : albedo.rgb) * emissiveFactor;
	
	vec3 color = initialAmbient * albedoRGB;
	float opac = albedo.w * opacity;
	if(useLight == 0 || !enableLights) {
		FragColor = vec4(albedoRGB, opac);
	} else {
		vec3 N = fs_in.normal;
		if (available[4]) {
			vec3 norm = texture(textures[4], texCoords).rgb;
			norm = normalize(norm * 2.0f - 1.0f);
			N = normalize(fs_in.TBN * norm);
		}
		//%lightColorAddition.glsl%

		FragColor = vec4(color, opac);
	}

	FragColor = FragColor + vec4(emissive, 0.0f);

	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
		BrightColor = FragColor;
	} else {
		BrightColor = vec4(0.0, 0.0, 0.0, opac) + vec4(emissive, 0.0f);
	}
}