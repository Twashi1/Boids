#version 460 core

layout(location = 0) out highp vec4 color;

// stolen from: https://stackoverflow.com/questions/64837705/opengl-blurring
in vec2 v_Pos;
uniform sampler2D u_Texture;
uniform vec2 u_TexSize;
uniform float u_Radius; // blur radius

void main()
{
	float x, y, xx, yy, rr = u_Radius * u_Radius, dx, dy, w, w0;
    w0 = 0.3780 / pow(u_Radius, 1.975);

    vec2 p;
    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);

    for (dx = 1.0 / u_TexSize.x, x = -u_Radius, p.x = 0.5 + (v_Pos.x * 0.5) + (x * dx); x <= u_Radius; x++, p.x += dx) {
        xx = x * x;

        for (dy = 1.0 / u_TexSize.y, y = -u_Radius, p.y = 0.5 + (v_Pos.y * 0.5) + (y * dy); y <= u_Radius; y++, p.y += dy) {
            yy = y * y;

            if (xx + yy <= rr) {
                w = w0 * exp((-xx - yy) / (2.0 * rr));
                col += texture2D(u_Texture, p) * w;
            }
        }
    }
    
    color = col;
}