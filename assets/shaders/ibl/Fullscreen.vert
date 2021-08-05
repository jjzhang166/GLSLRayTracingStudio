#version 330 core

precision highp float;

in vec3 position;
in vec2 uv;

out vec2 texCoord;

void main(void) 
{
    texCoord = uv;
    gl_Position = vec4(position, 1.0);
}