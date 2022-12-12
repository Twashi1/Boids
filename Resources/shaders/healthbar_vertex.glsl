#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec2 healthbarTexCoord;
layout(location = 3) in vec2 maskTexCoord;
layout(location = 4) in float healthValue;

uniform mat4 u_ProjMat;
uniform mat4 u_ViewMat;

out vec2 v_texCoord;
out vec2 v_healthbarTexCoord;
out vec2 v_maskTexCoord;
out float v_healthValue;

void main() {
	gl_Position = u_ProjMat * u_ViewMat * vec4(position.xy, 0.0, 1.0);
	v_texCoord = texCoord;
	v_healthbarTexCoord = healthbarTexCoord;
	v_maskTexCoord = maskTexCoord;
	v_healthValue = healthValue;
}