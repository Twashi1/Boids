#version 460 core

layout(location = 0) out highp vec4 color;

in vec2 v_texCoord;

uniform sampler2D u_Texture;
uniform float u_Scale = 1.0f;
uniform bool u_TookDamage = true; // TODO: should be varying, so can be per npc
// TODO: actually implement taking damage

void main()
{
	vec4 texColor = texture(u_Texture, v_texCoord / u_Scale);
	
	if (u_TookDamage) {
		// TODO: doesn't look good
		// texColor.r += 0.5f;
	}

	color = texColor;
}