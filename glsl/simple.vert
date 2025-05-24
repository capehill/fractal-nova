#version 310 es

in layout(location = 0) vec2 vertPos;
out vec4 colour;

const vec4 white = vec4(1.0);

void main()
{
    colour = white;
    gl_Position = vec4(vertPos, 0.0f, 1.0);
}

