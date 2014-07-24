#version 430 core

//just push through

layout(location = 0) in vec4 vPosition;

attribute vec4 gl_Color;

void main()
{
	gl_FrontColor	= gl_Color;
	gl_BackColor	= gl_Color;
	gl_Position		= vPosition;
}