#version 460 core

layout(location = 0) in vec4 position;

uniform mat4 u_ProjMat;
uniform mat4 u_ViewMat;

void main() {
	gl_Position = u_ProjMat * u_ViewMat * position;
}