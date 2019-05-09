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
} fs_in;

uniform sampler2D default_texture;

uniform sampler2D textures[6];
// ao, albedo, emissive, metallic, normal, roughness
//  0,      1,        2,        3,      4,         5
uniform bool available[6];
uniform mat4 model;
uniform float opacity;

//%lightComputations.glsl%

void main() {
	vec4 albedo = texture(available[1] ? textures[1] : default_texture, fs_in.texCoords);
	vec3 albedoRGB = albedo.rgb;
	float roughness = available[5] ? texture(textures[5], fs_in.texCoords).y : 1.0f;
	float metallic = available[3] ? texture(textures[3], fs_in.texCoords).y : 0.0f;
	float ao = available[0] ? texture(textures[0], fs_in.texCoords).y : 1.0f;
	vec3 emissive = available[2] ? texture(textures[2], fs_in.texCoords).rgb : vec3(0.0f, 0.0f, 0.0f);
	//todo: add AO and normal
	//vec3 normal = available[4] ? texture(textures[4], fs_in.texCoords).rgb : fs_in.normal;
	vec3 color = initialAmbient * albedoRGB;
	float opac = albedo.w * opacity;
	if(useLight == 0 || !enableLights) {
		FragColor = vec4(albedoRGB, opac);
	} else {
		vec3 N = fs_in.normal;
		vec3 V = normalize(fs_in.viewPosition - fs_in.pos);
		vec3 I = normalize(fs_in.pos - fs_in.viewPosition);

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