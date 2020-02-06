#version 310 es

in layout(location = 0) vec2 vertPos;
in layout(location = 1) vec2 textureCoord;

uniform layout(location = 0) float angle;
uniform layout(location = 1) float zoom;
uniform layout(location = 2) vec2 point;

out vec2 texCoord;

void main()
{
    texCoord = vec2(textureCoord.x * 3.5, textureCoord.y * 2.0);

    mat2 zoomMat = mat2(zoom, 0.0, 0.0, zoom);
    mat2 rotationMat = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));

    vec2 p = rotationMat * zoomMat * (vertPos + point);

    gl_Position = vec4(p, 0.0, 1.0);
}

