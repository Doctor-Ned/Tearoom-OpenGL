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
	vec3 normal;
	vec3 viewPosition;
	vec4 fragDirSpaces[MAX_LIGHTS_OF_TYPE];
	vec4 fragSpotSpaces[MAX_LIGHTS_OF_TYPE];
} fs_in;

uniform vec4 color;
uniform mat4 model;
uniform float opacity;

//%lightComputations.glsl%

void main() {
	vec4 diffuse = tcolor * color;
    vec3 ambient = initialAmbient * diffuse.rgb;
	if(useLight == 0) {
		FragColor = vec4(color.rgb, opacity);
	} else {
		vec3 specular = vec3(0.5f);
		vec3 viewDir = normalize(fs_in.viewPosition - fs_in.pos);

		vec3 color = ambient;

		//%lightColorAddition.glsl%

		FragColor = vec4(color, opacity*diffuse.w);
	}
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
		BrightColor = FragColor;
	} else {
		BrightColor = vec4(0.0, 0.0, 0.0, opacity);
	}
}