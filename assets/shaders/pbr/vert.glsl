#version 330 core

precision highp float;
precision highp int;
precision highp sampler2D;
precision highp samplerCube;
precision highp isampler2D;
precision highp sampler2DArray;

layout (location = 0) in vec3 inPositions;
layout (location = 1) in uint inNormals;
layout (location = 2) in vec2 inTexCoords;
layout (location = 3) in uint inTangents;
layout (location = 4) in uint inColors;

uniform mat4 mvp;

out vec2 varyTexCoords;
out vec3 varyNormals;

void main()
{
    // normal
    vec3 normal;
    {
        int red   = int(inNormals >> 20) & 1023;
        int green = int(inNormals >> 10) & 1023;
        int blue  = int(inNormals >>  0) & 1023;

        normal.xyz = vec3(red / 1024.0f, green / 1024.0f, blue / 1024.0f) * 2.0f - 1.0f;
    }

    // tangent
    vec4 tangent;
    {
        int alpha = int(inTangents >> 30) & 1023;
        int red   = int(inTangents >> 20) & 1023;
        int green = int(inTangents >> 10) & 1023;
        int blue  = int(inTangents >>  0) & 1023;

        tangent.xyz = vec3(red / 1024.0f, green / 1024.0f, blue / 1024.0f) * 2.0f - 1.0f;
        tangent.w   = alpha > 0 ? 1.0f : -1.0f;
    }
    // color
    vec4 color;
    {
        int red   = int(inColors >> 24) & 256;
        int green = int(inColors >> 16) & 256;
        int blue  = int(inColors >> 8)  & 256;
        int alpha = int(inColors >> 0)  & 256;
        color.xyzw = vec4(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f);
    }

    gl_Position   = mvp * vec4(inPositions, 1.0);
	varyTexCoords = inTexCoords.xy;
    varyNormals   = normal;
}