#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;
uniform vec3 blurScale;

void main() {
	vec4 color = vec4(0.0);

	color += texture2D(image, texCoord0 + (vec2(-3.0) * blurScale.xy)) * (1.0 / 64.0);
	color += texture2D(image, texCoord0 + (vec2(-2.0) * blurScale.xy)) * (6.0 / 64.0);
	color += texture2D(image, texCoord0 + (vec2(-1.0) * blurScale.xy)) * (15.0 / 64.0);
	color += texture2D(image, texCoord0 + (vec2(0.0) * blurScale.xy))  * (20.0 / 64.0);
	color += texture2D(image, texCoord0 + (vec2(1.0) * blurScale.xy))  * (15.0 / 64.0);
	color += texture2D(image, texCoord0 + (vec2(2.0) * blurScale.xy))  * (6.0 / 64.0);
	color += texture2D(image, texCoord0 + (vec2(3.0) * blurScale.xy))  * (1.0 / 64.0);

	color.a = 1.0f;
	FragColor = color;
}