#version 460 core

layout(location = 0) out highp vec4 color;

in vec2 v_texCoord;

uniform vec3 u_Color;

void main()
{
    vec2 value = v_texCoord * 2.0f - vec2(1.0f, 1.0f);

    // https://stackoverflow.com/questions/22444450/drawing-circle-with-opengl
	float R = 1.0f;
    float R2 = 0.95f;
    float dist = sqrt(dot(value,value));
    if (dist >= R || dist <= R2) {
        discard;
    }
    float sm = smoothstep(R,R-0.01,dist);
    float sm2 = smoothstep(R2,R2+0.01,dist);
    float alpha = sm*sm2;

    color = vec4(u_Color.xyz, alpha);
}