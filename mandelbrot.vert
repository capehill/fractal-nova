#version 310 es

in layout(location = 0) vec2 vertPos;
in layout(location = 1) vec2 textureCoord;

uniform layout(location = 0) float angle;

out vec2 texCoord;

void main()
{
    texCoord = textureCoord;

    mat2 m = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    vec2 p = m * vertPos;

    gl_Position = vec4(p, 0.0, 1.0);

    //float x = vertPos.x * cos(angle) - vertPos.y * sin(angle);
    //float y = vertPos.x * sin(angle) + vertPos.y * cos(angle);

    //gl_Position = vec4(x, y, 0.0, 1.0);
    //gl_Position = vec4(vertPos, 0.0, 1.0);
}

