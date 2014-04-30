// vertex shader
#version 430

layout(location = 0) in vec4 pos;

// global uniform block
layout(std140) uniform TransformMat
{
	mat4 camera;
	mat4 proj;
};

uniform mat4 world;
uniform vec4 cubeColor;

smooth out vec4 vertexColor;

void main()
{
	gl_Position = proj * camera * world * pos;
	vertexColor = cubeColor;
}