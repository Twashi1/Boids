#version 460 core

// Code for cellular noise from:
// https://docs.unity3d.com/Packages/com.unity.shadergraph@6.9/manual/Voronoi-Node.html#:~:text=Voronoi%20noise%20is%20generated%20by,controlled%20by%20input%20Cell%20Density.
// Idea from:
// https://www.youtube.com/watch?v=Vg0L9aCRWPE&ab_channel=Brackeys
// Pixelation from:
// https://godotshaders.com/shader/pixelate/

layout(location = 0) out highp vec4 color;

uniform float u_Scale = 1.0f;
uniform uint  u_Seed = 0;
uniform float u_AngularVelocity = 0.5f;
uniform float u_Time = 0.0f;
uniform float u_CellularDensity = 3.0f;
uniform float u_Sharpness = 5.0f;
uniform float u_ColorGamma = 1.75f; // TODO: Gamma seems like an appropriate word here, but I'm not sure
uniform float u_LuminosityOffset = 0.5f;
uniform vec3  u_BaseColor = vec3(0.9f, 0.1f, 0.1f); // Used American spelling for RGBColor, so I'll do the same here

uniform bool  u_HorizontalWave = false; // TODO: not implemented
uniform bool  u_VerticalWave = true; // Wave on right hand side
uniform float u_WaveAmplitude = 0.05f;
uniform float u_WobbleAmplitude = 0.005f;
uniform float u_WobbleSpeed = 0.6f;
uniform float u_WaveOffset = 1.0f - 0.05f - 0.005f;
uniform float u_Wavelength = 5.0f;
uniform float u_Wavespeed = 0.2f;
uniform float u_PI = 3.14159265358f;

uniform float u_Pixelation = 40.0f;
uniform float u_LessPixelation = 160.0f;

uniform sampler2D u_Texture;

in vec2 v_texCoord;
in vec2 v_healthbarTexCoord;
in vec2 v_maskTexCoord;
in float v_healthValue;

vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p,vec2(127.1 + float(u_Seed),311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

vec2 rotated_random(vec2 p, mat2x2 rot_matrix) {
    const vec2 pivot = vec2(0.5f, 0.5f);

    vec2 random = random2(p) - pivot;
    random *= rot_matrix;
    random += pivot;

    return random;
}

float random_float (float h) {
    return fract(sin(h) * 43758.5453123);
}

vec4 get_fluid_texture() {
    float alpha = 1.0f;
    vec2 pixelated_uv = round(v_texCoord * u_Pixelation) / u_Pixelation;
    vec2 less_pixelated_uv = round(v_texCoord * u_LessPixelation) / u_LessPixelation;

    if (u_VerticalWave) {
        float radian_convert = 2 * u_PI;

        float noise_real_pos = (pixelated_uv.y + u_Time * u_Wavespeed) * u_Wavelength;
        float noise_coord = floor(noise_real_pos);
        float noise_speed = fract(noise_real_pos);

        // Calculate noise value by interpolating with smoothstep between two random floats
        float noise_value = mix(random_float(noise_coord), random_float((noise_coord + 1.0f)), smoothstep(0.0f, 1.0f, noise_speed)) * u_WaveAmplitude;
        // Add small wobble to the wave so it's not just translating
        // TODO: this wobble should be noise as well, not just a cos graph
        float wobble = cos((pixelated_uv.y + fract(u_Time * u_WobbleSpeed)) * radian_convert) * 0.5f * u_WobbleAmplitude;

        // TODO: better name
        // NOTE: multiply by -1 to get it on the left side
        // Value will be negative to the right of the wave, and positive to the left
        float cutoff = (noise_value + wobble) - less_pixelated_uv.x + u_WaveOffset + v_healthValue;
        
        // Don't bother calculating anything else if we're on the right of the wave
        // Otherwise we let the alpha value default to 1
        // TODO: some blur/bleed on edge that doesn't look ugly
        if (cutoff < 0.0f) { alpha = 0.0f; }
    }

    // Get angle
    float angle = u_AngularVelocity * u_Time;

    // Compute cos and sin of angle
    float cos_angle = cos(angle);
    float sin_angle = sin(angle);

    // Rotation matrix
    mat2x2 rot_matrix;
    rot_matrix[0] = vec2(cos_angle, -sin_angle);
    rot_matrix[1] = vec2(sin_angle,  cos_angle);

    vec2 scaled_coords = pixelated_uv * u_CellularDensity;
    vec2 current_poll_lattice = floor(scaled_coords);
    vec2 current_poll_pos = fract(scaled_coords); // Position relative to lattice

    // Is a vec3 so we can pass voronoi stuff if needed in future
    vec3 result = vec3(1000.0f, 0.0f, 0.0f);

    // Iterate each surrounding cell
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            vec2 lattice_offset = vec2(x, y);
            vec2 random_point = rotated_random(lattice_offset + current_poll_lattice, rot_matrix);

            float dist = distance(lattice_offset + random_point, current_poll_pos);

            if (dist < result.x) {
                result = vec3(dist, random_point.xy);
            }
        }
    }

    result.x = pow(result.x, u_Sharpness);

    vec3 result_color = pow(u_BaseColor * (result.x + u_LuminosityOffset), vec3(u_ColorGamma));

    return vec4(result_color.xyz, alpha);
}

void main() {
    float alpha = 1.0f;

    vec4 healthbar_color = texture2D(u_Texture, v_healthbarTexCoord);
    vec4 mask_color = texture2D(u_Texture, v_maskTexCoord);
    vec4 fluid_color = get_fluid_texture();

    color = healthbar_color;
    
    if (fluid_color.a > 0.0f && mask_color.a < 1.0f) {
        color = fluid_color;
    }
}