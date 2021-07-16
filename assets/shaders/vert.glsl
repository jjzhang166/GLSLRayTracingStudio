#version 330

precision highp float;
precision highp int;
precision highp sampler2D;
precision highp samplerCube;
precision highp isampler2D;
precision highp sampler2DArray;

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inTexCoords;

out vec2 varyTexCoords;

void main()
{
    gl_Position   = vec4(inPosition.x, inPosition.y, 0.0, 1.0);
	varyTexCoords = inTexCoords;
}