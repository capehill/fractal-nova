#version 310 es

precision mediump float;

uniform layout(location = 0) vec2 u_dimension;
uniform layout(location = 1) vec2 u_point;
uniform layout(location = 2) vec2 u_scale;

//in layout(location = 0) vec4 color;

out vec4 fragColor;

void main()
{
    float _x = 1.0 / u_dimension.x;
    float _y = 1.0 / u_dimension.y;
    float x0 = (_x * gl_FragCoord.x * 3.5) / u_scale.x + u_point.x;
    float y0 = (_y * gl_FragCoord.y * 2.0) / u_scale.y + u_point.y;
    const int maxIterations = 100;

    float x = 0.0;
    float y = 0.0;
    int iteration = 0;
    float xx = 0.0;
    float yy = 0.0;

    while (((xx + yy) <= 4.0) && (iteration < maxIterations)) {
        xx = x * x;
        yy = y * y;
        float xtemp = xx - yy + x0;
        y = 2.0 * x * y + y0;
        x = xtemp;
        iteration++;
    }

    //fragColor = vec4(float(iteration) / float(maxIterations), xx / 2.0, yy / 2.0, 1.0);
    fragColor = vec4(1.0 - float(iteration) / float(maxIterations));
}
