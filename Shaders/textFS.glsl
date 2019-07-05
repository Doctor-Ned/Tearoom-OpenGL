#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

uniform sampler2D text;
uniform vec4 color;
uniform float opacity;
uniform vec3 emissive;

in VS_OUT{
	vec3 pos;
	vec2 texCoords;
} fs_in;

void main()
{
	float opac = opacity * texture(text, fs_in.texCoords).r;
	FragColor = vec4(color.xyz + emissive, color.w * opac);
	BrightColor = vec4(emissive, opac);
}  