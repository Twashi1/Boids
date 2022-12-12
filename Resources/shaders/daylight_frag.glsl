#version 460 core

layout(location = 0) out highp vec4 color;

in vec2 v_texCoord;

uniform sampler2D u_Texture;
uniform float u_Scale = 1.0f;
uniform float u_Time;
uniform float u_PI = 3.14159265;

float s_DAYLIGHT_RATIO = 0.3f;
float s_NIGHT_RATIO = 0.1f;
float s_MIN = 0.35f;
float s_MAX = 1.0f;
float s_RANGE = s_MAX - s_MIN;
float s_PERIOD = 300.0f; // 5 min night cycle, 3/10 is day, 1/10 is night, rest is transition time

void main()
{
	// Will get time on scale 0 -> 1
	float time = (sin(fract(u_Time / s_PERIOD) * 2.0f * u_PI) + 1.0f) * 0.5f;
	// Move to scale s_MIN - s_NIGHT_RATIO -> s_MAX + s_DAYLIGHT_RATIO
	float lighting_scale = (time * (s_RANGE + s_NIGHT_RATIO + s_DAYLIGHT_RATIO)) + (s_MIN - s_NIGHT_RATIO);
	// Clamp
	lighting_scale = max(min(lighting_scale, s_MAX), s_MIN);

	vec4 texColor = texture(u_Texture, v_texCoord / u_Scale) * lighting_scale;
	color = texColor;
}