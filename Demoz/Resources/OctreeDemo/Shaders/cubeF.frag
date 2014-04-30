// vertex shader
#version 430

out vec4 color;

smooth in vec4 vertexColor;

void main()
{
    //set the color the fragment to color
    color = vertexColor;
}