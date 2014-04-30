// vertex shader
#version 430

layout(location = 0) in vec3 vert;

// global uniform block
layout(std140) uniform TransformMat
{
	mat4 camera;
	mat4 proj;
};

uniform mat4 world;

void main()
{
	gl_Position = proj * camera * world * vec4(vert, 1.0);
}