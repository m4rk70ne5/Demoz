// vertex shader
#version 430

uniform vec3 inColor;

out vec4 color;

void main()
{
    //set the color the fragment to color
    color = vec4(inColor.x, inColor.y, inColor.z, 1.0);
}