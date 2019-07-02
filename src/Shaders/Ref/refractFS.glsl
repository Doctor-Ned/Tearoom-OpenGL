#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

uniform samplerCube environmentMap;

in VS_OUT {
	vec3 pos;
	vec3 normal;
	vec3 viewPosition;
} fs_in;

void main() {
	vec3 viewDirection = normalize(fs_in.pos - fs_in.viewPosition);
	vec3 refraction = refract(viewDirection, fs_in.normal, 1.00/1.52);
	FragColor = vec4(texture(environmentMap, refraction).rgb, 1.0);
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
		BrightColor = FragColor;
	} else {
		BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
	}
}