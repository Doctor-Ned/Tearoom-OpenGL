#version 430
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
	vec4 fragDirSpaces[MAX_LIGHTS_OF_TYPE][LIGHT_SPLITS];
	vec4 fragSpotSpaces[MAX_LIGHTS_OF_TYPE];
} fs_in;

uniform sampler2D texture_diffuse1;
uniform mat4 model;
uniform float opacity;
uniform float roughness;
uniform float metallic;
uniform vec3 emissive;
const float ao = 1.0f;

//%lightComputations.glsl%

void main() {
	vec4 albedo = texture(texture_diffuse1, fs_in.texCoords);
	vec3 albedoRGB = albedo.rgb;
	float opac = opacity * albedo.w;
	vec3 color = initialAmbient * albedoRGB;
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