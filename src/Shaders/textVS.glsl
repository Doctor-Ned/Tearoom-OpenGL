#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

uniform mat4 model;
uniform vec3 viewPosition;

layout(std140) uniform ViewProjection{
	mat4 view;
	mat4 projection;
};

out VS_OUT{
	vec3 pos;
	vec2 texCoords;
} vs_out;

void main()
{
	vs_out.pos = vec3(model * vec4(vertex.xy, 0.0f, 1.0f));
	gl_Position = projection * view * vec4(vs_out.pos, 1.0f);
    vs_out.texCoords = vertex.zw;
} 