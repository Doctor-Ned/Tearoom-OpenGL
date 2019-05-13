#version 430
layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texCoords;

out vec2 exPos;
out vec2 exTexCoords;

void main()
{
	exTexCoords = texCoords;
    gl_Position = vec4(vertex, 0.0f, 1.0f);
} 