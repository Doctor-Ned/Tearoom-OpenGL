#version 330 core

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
uniform mat4 model;

out vec4 outColor;

//%lightComputations.glsl%

void main() {
	vec3 diffuse = tcolor.rgb;
	if(!disableTexture) diffuse = diffuse * texture(texture_diffuse1, fs_in.texCoords).rgb;
    vec3 ambient = initialAmbient * diffuse;
	if(useLight == 0) {
		outColor = vec4(diffuse, 1.0f);
	} else {
		vec3 specular = vec3(0.5f);
		vec3 viewDir = normalize(fs_in.viewPosition - fs_in.pos);

		vec3 color = ambient;

		//%lightColorAddition.glsl%

		outColor = vec4(color, 1.0f);
	}
}