#version 460 core

layout(location = 0) out highp vec4 color;

in vec2 v_texCoord;
in float v_SlotAlpha;

uniform sampler2D u_Texture;
uniform float u_Scale = 1.0f;

void main()
{
	vec4 texColor = texture(u_Texture, v_texCoord / u_Scale);
	// TODO: some weird stuff happening "shader recompilation"?
	color = vec4(texColor.xyz, texColor.w * v_SlotAlpha);
}