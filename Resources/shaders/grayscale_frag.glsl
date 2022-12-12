#version 460 core

layout(location = 0) out highp vec4 color;

in vec2 v_texCoord;

uniform sampler2D u_Texture;
uniform float u_Scale = 1.0f;

void main()
{
	vec4 texColor = texture(u_Texture, v_texCoord / u_Scale);
	float luminosity = (texColor.x + texColor.y + texColor.z) / 3.0;
	color = vec4(luminosity, luminosity, luminosity, texColor.w);
}