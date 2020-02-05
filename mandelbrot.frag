#version 310 es

precision mediump float; // TODO: high?

uniform layout(location = 0) vec2 u_factor;
uniform layout(location = 1) vec2 u_point;
uniform layout(location = 2) int u_iterations;

uniform layout(binding = 0) sampler2D texSampler;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
//    float x0 = u_factor.x * gl_FragCoord.x + u_point.x;
//    float y0 = u_factor.y * gl_FragCoord.y + u_point.y;
//    float x0 = u_factor.x * texCoord.x + u_point.x;
//    float y0 = u_factor.y * texCoord.y + u_point.y;

    float x0 = 3.5 * texCoord.x;
    float y0 = 2.0 * texCoord.y;

    const int maxIterations = 1000;

    float x = 0.0;
    float y = 0.0;
    int iteration = 0;
    float xx = 0.0;
    float yy = 0.0;

    while (((xx + yy) <= 4.0) && (iteration < u_iterations)) {
    //while (((xx + yy) <= 4.0) && (iteration < maxIterations)) {
        xx = x * x;
        yy = y * y;
        float xtemp = xx - yy + x0;
        y = 2.0 * x * y + y0;
        x = xtemp;
        iteration++;
    }

    //fragColor = vec4(1.0 - float(iteration) / float(maxIterations));
    fragColor = texture(texSampler, vec2(float(iteration) / float(u_iterations), 0.0));
    //fragColor = texture(texSampler, vec2(float(iteration) / 100.0, 0.0));
}
