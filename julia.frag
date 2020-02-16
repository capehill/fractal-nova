#version 310 es

precision highp float;

uniform layout(location = 0) int u_iterations;

uniform layout(binding = 0) sampler2D texSampler;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
    const vec2 c = vec2(0.285, 0.00);

    float x = texCoord.x;
    float y = texCoord.y;
    int iteration = 0;
    float xx = 0.0;
    float yy = 0.0;

    while (((xx + yy) <= 4.0) && (iteration < u_iterations)) {
        xx = x * x;
        yy = y * y;
        float xtemp = xx - yy;
        y = 2.0 * x * y + c.y;
        x = xtemp + c.x;
        iteration++;
    }

    fragColor = texture(texSampler, vec2(float(iteration) / float(u_iterations), 0.0));
    //fragColor = texture(texSampler, vec2(float(iteration) / 100.0, 0.0));
}
