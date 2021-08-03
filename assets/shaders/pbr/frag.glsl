#version 330

precision highp float;
precision highp int;
precision highp sampler2D;
precision highp samplerCube;
precision highp isampler2D;
precision highp sampler2DArray;

in vec2 varyTexCoords;
in vec3 varyNormals;

out vec4 outColor;

void main()
{
	outColor = vec4(varyNormals.x, varyNormals.y, varyNormals.z, 1.0);
}