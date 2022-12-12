#version 460 core

layout(location = 0) out highp vec4 color;

in vec2 v_texCoord;

void main()
{
	color = vec4(v_texCoord.xy, 0.0, 1.0);
}