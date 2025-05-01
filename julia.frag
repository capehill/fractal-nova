#version 310 es

precision highp float;

uniform layout(location = 0) int u_iterations;
uniform layout(location = 1) vec2 u_complex;

uniform layout(binding = 0) sampler2D texSampler;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
    float x = texCoord.x;
    float y = texCoord.y;
    int iteration = 0;
    float xx = 0.0;
    float yy = 0.0;

    float i = exp(-length(texCoord));

    while (((xx + yy) <= 4.0) && (iteration < u_iterations)) {
        xx = x * x;
        yy = y * y;
        float xtemp = xx - yy;
        y = 2.0 * x * y + u_complex.y;
        x = xtemp + u_complex.x;
        iteration++;
        i += exp(-length(vec2(x, y)));
    }

    fragColor = texture(texSampler, vec2(i / log2(float(u_iterations)), 0.0));
}
