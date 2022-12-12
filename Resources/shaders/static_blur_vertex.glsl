#version 460 core

layout(location = 0) in vec4 position;

uniform mat4 u_ProjMat;
uniform mat4 u_ViewMat;

out vec2 v_Pos;

void main() {
	v_Pos = (u_ProjMat * position).xy;
	gl_Position = u_ProjMat * position;
}