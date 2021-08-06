#version 330 core

precision highp float;
precision highp int;
precision highp sampler2D;
precision highp samplerCube;
precision highp isampler2D;
precision highp sampler2DArray;

uniform samplerCube _GGXEnvSampler;
uniform float _Exposure;
uniform float _GammaValue;
uniform float _EnvironmentLod;
uniform float _EnvironmentIntensity;

in vec3 varyUVW;

out vec4 outColor;

vec3 sRGBToLinear(vec3 srgbIn)
{
    return vec3(pow(srgbIn.xyz, vec3(_GammaValue)));
}

vec3 LinearTosRGB(vec3 color)
{
    return pow(color, vec3(1.0 / _GammaValue));
}

vec3 ToneMapACES(vec3 color)
{
    float A = 2.51;
    float B = 0.03;
    float C = 2.43;
    float D = 0.59;
    float E = 0.14;
    return LinearTosRGB(clamp((color * (A * color + B)) / (color * (C * color + D) + E), 0.0, 1.0));
}

vec3 ToneMap(vec3 color)
{
    color *= _Exposure;
    return ToneMapACES(color);
}

void main()
{
    vec4 color = textureLod(_GGXEnvSampler, varyUVW, _EnvironmentLod).xyzw;

    color.rgb  = sRGBToLinear(color.rgb);
    color.rgb *= _EnvironmentIntensity;

    // tonemapping
    color.rgb = ToneMap(color.rgb);
    color.a   = 1.0;

    outColor  = color;
}