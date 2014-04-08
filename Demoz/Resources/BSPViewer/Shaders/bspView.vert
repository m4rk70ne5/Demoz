#version 430

layout(location = 0) in vec4 position; //input vertex coordinates
layout(location = 1) in vec2 vTexCoord; //input texture coordinates
uniform mat4 proj;
uniform mat4 camera;
out vec2 gTexCoord;

void main()
{
	gl_Position = proj * camera * position;
	gTexCoord = vTexCoord;
}