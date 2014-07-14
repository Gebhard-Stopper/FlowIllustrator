#version 430 core

//just push through

uniform sampler2D tex;

uniform int ExtentX;
uniform int ExtentY;

in vec2 vs_tex_coord;

//attribute vec4 gl_Color;

layout (location = 0) out vec4 color;

void main()
{
	 color=texture(tex, vec2( gl_FragCoord.x/ExtentX, gl_FragCoord.y/ExtentY) );
}