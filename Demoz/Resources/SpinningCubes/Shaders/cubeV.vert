// vertex shader
#version 430

layout(location = 0) in vec4 pos;

uniform vec4 cubeColor;

uniform mat4 proj;
uniform mat4 camera;
uniform mat4 world;

smooth out vec4 vertexColor;

void main()
{
	gl_Position = proj * camera * world * pos;
	vertexColor = cubeColor;
}