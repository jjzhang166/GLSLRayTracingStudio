#version 330 core

precision highp float;

in vec2 position;
in vec2 uv;

out vec2 texCoord;

void main(void) 
{
    texCoord = uv;
    gl_Position = vec4(position.x, position.y, 1.0, 1.0);
}