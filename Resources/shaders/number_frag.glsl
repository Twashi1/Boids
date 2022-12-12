#version 460 core
in vec2 v_TexCoords;
out vec4 color;

uniform sampler2D u_Texture;
uniform vec3 u_TextColor = vec3(1.0, 1.0, 1.0);

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Texture, v_TexCoords).r);
    color = vec4(u_TextColor, 1.0) * sampled;
}