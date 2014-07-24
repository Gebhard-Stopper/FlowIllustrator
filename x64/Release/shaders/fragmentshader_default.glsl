#version 430 core

//Push through the color

varying vec4 gl_Color; 
in vec4 color;

void main()
{
	gl_FragColor = gl_Color;
}