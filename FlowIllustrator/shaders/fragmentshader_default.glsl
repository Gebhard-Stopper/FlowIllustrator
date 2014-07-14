#version 430 core

//Push through the color

varying vec4 gl_Color; 

void main()
{
	gl_FragColor = gl_Color;
}