#version 310 es

precision highp float;

uniform layout(location = 0) int u_iterations;

uniform layout(binding = 0) sampler2D texSampler;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
    float x0 = texCoord.x;
    float y0 = texCoord.y;

    float x = 0.0;
    float y = 0.0;
    int iteration = 0;
    float xx = 0.0;
    float yy = 0.0;

    while (((xx + yy) <= 4.0) && (iteration < u_iterations)) {
        xx = x * x;
        yy = y * y;
        float xtemp = xx - yy + x0;
        y = 2.0 * x * y + y0;
        x = xtemp;
        iteration++;
    }

    float i = float(iteration) + 1.0 - log(log(length(vec2(x, y)))) / log(2.0);
    fragColor = texture(texSampler, vec2(i / float(u_iterations), 0.0));
}


