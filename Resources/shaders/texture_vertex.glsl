#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 u_ProjMat;
uniform mat4 u_ViewMat;

out vec2 v_texCoord;

void main() {
	gl_Position = u_ProjMat * u_ViewMat * position;
	v_texCoord = texCoord;
}