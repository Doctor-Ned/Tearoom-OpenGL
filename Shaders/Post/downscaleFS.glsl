#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;
uniform ivec2 row;
uniform ivec2 col;
uniform vec2 inverseSourceSize;

void main() {
	const vec3 luma = vec3(0.299, 0.587, 0.114);
	vec4 best = texture2D(image, TexCoords);
	float maxLuma = dot(luma, best.rgb);

	for (int i = row[0]; i < row[1]; i++) {
		for (int j = col[0]; j < col[1]; j++) {
			vec4 curr = texture2D(image, TexCoords + vec2(i, j) * inverseSourceSize);
			float currLuma = dot(luma, curr.rgb);
			if (currLuma > maxLuma) {
				best = curr;
				maxLuma = currLuma;
			}
		}
	}

	FragColor = best;
}