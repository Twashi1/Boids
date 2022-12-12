#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;
//layout(location = 2) in float itemCount;

uniform mat4 u_ProjMat;
uniform mat4 u_ViewMat;

out vec2 v_texCoord;
//out int v_ItemCount;

void main() {
	gl_Position = u_ProjMat * u_ViewMat * vec4(position, 0.0f, 1.0f);
	v_texCoord = texCoord;
	//v_ItemCount = int(itemCount);
}