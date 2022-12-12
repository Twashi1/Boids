#version 460 core

layout(location = 0) out highp vec4 color;

uniform vec4 u_Color;

void main()
{
	color = u_Color;
}