#version 430

layout (location = 0) in vec3 inPosition;

out vec3 texCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
	texCoords = inPosition;
	vec4 pos = projection * view * vec4(inPosition, 1.0f);
	gl_Position = pos.xyww;
}