#version 330 core

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
	vec3 normal = (varyNormals + vec3(1.0, 1.0f, 1.0)) * 0.5;
	outColor = vec4(normal.x, normal.y, normal.z, 1.0);
}