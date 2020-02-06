#version 310 es

precision highp float;

//uniform layout(location = 0) vec2 u_factor;
//uniform layout(location = 1) vec2 u_point;
uniform layout(location = 0) int u_iterations;

uniform layout(binding = 0) sampler2D texSampler;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
//    float x0 = u_factor.x * texCoord.x + u_point.x;
//    float y0 = u_factor.y * texCoord.y + u_point.y;

    //vec2 c0 = texCoord;
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

    fragColor = texture(texSampler, vec2(float(iteration) / float(u_iterations), 0.0));
    //fragColor = texture(texSampler, vec2(float(iteration) / 100.0, 0.0));
}
