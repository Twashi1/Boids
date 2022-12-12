#version 460 core

layout(location = 0) out highp vec4 color;

in vec2 v_texCoord;
uniform vec3 u_BarColor = vec3(0.5, 0.5, 0.5);
uniform vec3 u_ShadedColor = vec3(1.0, 0.0, 0.0);
uniform float u_Value = 0.0;

void main()
{
	if (v_texCoord.x <= u_Value) {
		color = vec4(u_ShadedColor, 1.0);
	} else {
		color = vec4(u_BarColor, 1.0);
	}
}