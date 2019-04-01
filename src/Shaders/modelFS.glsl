#version 330 core

layout (std140) uniform TextureColor {
	vec4 tcolor;
	bool disableTexture;
};

//%lights.sglsl%

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
uniform int useSpecularMap;

out vec4 outColor;

//%lightComputations.sglsl%

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