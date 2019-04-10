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

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform mat4 model;
uniform float opacity;
uniform int useSpecularMap;

//%lightComputations.glsl%

void main() {
	vec4 diffuse = vec4(tcolor.rgb, 1.0f);
	if(!disableTexture) diffuse *= texture(texture_diffuse1, fs_in.texCoords);
    vec4 ambient = initialAmbient * diffuse;
	ambient.w = diffuse.w;
	if(useLight == 0) {
		FragColor = vec4(diffuse.rgb, diffuse.w * opacity);
	} else {
		vec3 specular = useSpecularMap > 0 ? texture(texture_specular1, fs_in.texCoords).rgb : vec3(0.5f);
		vec3 viewDir = normalize(fs_in.viewPosition - fs_in.pos);

		vec3 color = ambient.rgb;

		//%lightColorAddition.glsl%

		FragColor = vec4(color, ambient.w * opacity);
	}
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
		BrightColor = FragColor;
	} else {
		BrightColor = vec4(0.0, 0.0, 0.0, opacity);
	}
}