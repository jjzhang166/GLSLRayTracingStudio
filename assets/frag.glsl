#version 330

precision highp float;
precision highp int;
precision highp sampler2D;
precision highp samplerCube;
precision highp isampler2D;
precision highp sampler2DArray;

out vec4 outColor;

in vec2 varyTexCoords;

uniform sampler2D mainTexture;

void main()
{
	outColor = texture(mainTexture, varyTexCoords);
}