#version 330 core

precision highp float;
precision highp int;
precision highp sampler2D;
precision highp samplerCube;
precision highp isampler2D;
precision highp sampler2DArray;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec4 inTangent;
layout (location = 4) in vec4 inColor;

uniform mat4 _MVP;

out vec2 varyTexCoords;
out vec3 varyNormals;

void main()
{
    gl_Position   = _MVP * vec4(inPosition, 1.0);
	varyTexCoords = inTexCoord.xy;
    varyNormals   = normalize(inNormal);
}