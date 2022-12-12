#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in float alpha;
layout(location = 3) in float rotation;
layout(location = 4) in vec2 center;

uniform mat4 u_ProjMat;
uniform mat4 u_ViewMat;

out vec2 v_texCoord;
out float v_Alpha;

void main() {
	mat2x2 rotation_matrix;
	float sin_rot = sin(rotation);
	float cos_rot = cos(rotation);

	/*
	Rotation matrix:
	 cosA    -sinA    00 10
	 sinA     cosA    01 11
	*/

	rotation_matrix[0][0] = cos_rot;
	rotation_matrix[1][0] = -sin_rot;
	rotation_matrix[0][1] = sin_rot;
	rotation_matrix[1][1] = cos_rot;

	vec2 rotated_pos = (rotation_matrix * (position.xy - center)) + center;

	gl_Position = u_ProjMat * u_ViewMat * vec4(rotated_pos.xy, 0.0, 1.0);
	v_texCoord = texCoord;
	v_Alpha = alpha;
}