#version 310 es

in layout(location = 0) vec2 vertPos;

void main()
{
    gl_Position = vec4(vertPos, 0.0f, 1.0);
}

