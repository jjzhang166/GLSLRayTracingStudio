#version 330 core
precision highp float;

#define MATH_PI 3.1415926535897932384626433832795

const int TypeLambertian = 0;
const int TypeGGX = 1;
const int TypeCharlie = 2;

uniform samplerCube _InputCubeMap;
uniform float _Roughness;
uniform int _SampleCount;
uniform int _Width;
uniform float _LodBias;
uniform int _Distribution;
uniform int _CurrentFace;
uniform int _GeneratingLUT;

in vec2 texCoord;
out vec4 fragmentColor;

vec3 UVToXYZ(int face, vec2 uv)
{
    if (face == 0)
    {
        return vec3(1.0f, uv.y, -uv.x);
    }
    else if (face == 1)
    {
        return vec3(-1.0f, uv.y, uv.x);
    }
    else if (face == 2)
    {
        return vec3(+uv.x, -1.0f, +uv.y);
    }
    else if (face == 3)
    {
        return vec3(+uv.x, 1.0f, -uv.y);
    }
    else if (face == 4)
    {
        return vec3(+uv.x, uv.y, 1.0f);
    }
    else 
    {
        return vec3(-uv.x, +uv.y, -1.0f);
    }
}

vec2 DirToUV(vec3 dir)
{
    return vec2(0.5f + 0.5f * atan(dir.z, dir.x) / MATH_PI, 1.0f - acos(dir.y) / MATH_PI);
}

float Saturate(float v)
{
    return clamp(v, 0.0f, 1.0f);
}

float RadicalInverseVdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 Hammersley2d(int i, int n) 
{
    return vec2(float(i) / float(n), RadicalInverseVdC(uint(i)));
}

mat3 GenerateTBN(vec3 normal)
{
    vec3 bitangent = vec3(0.0, 1.0, 0.0);
    float NdotUp = dot(normal, vec3(0.0, 1.0, 0.0));
    float epsilon = 0.0000001;
    
    if (1.0 - abs(NdotUp) <= epsilon)
    {
        if (NdotUp > 0.0)
        {
            bitangent = vec3(0.0, 0.0, 1.0);
        }
        else
        {
            bitangent = vec3(0.0, 0.0, -1.0);
        }
    }

    vec3 tangent = normalize(cross(bitangent, normal));
    bitangent = cross(normal, tangent);

    return mat3(tangent, bitangent, normal);
}

float D_GGX(float NdotH, float roughness) 
{
    float a = NdotH * roughness;
    float k = roughness / (1.0 - NdotH * NdotH + a * a);
    return k * k * (1.0 / MATH_PI);
}

struct MicrofacetDistributionSample
{
    float pdf;
    float cosTheta;
    float sinTheta;
    float phi;
};

MicrofacetDistributionSample GGX(vec2 xi, float roughness)
{
    MicrofacetDistributionSample ggx;

    float alpha = roughness * roughness;
    ggx.cosTheta = Saturate(sqrt((1.0 - xi.y) / (1.0 + (alpha * alpha - 1.0) * xi.y)));
    ggx.sinTheta = sqrt(1.0 - ggx.cosTheta * ggx.cosTheta);
    ggx.phi = 2.0 * MATH_PI * xi.x;
    ggx.pdf = D_GGX(ggx.cosTheta, alpha);
    ggx.pdf /= 4.0;

    return ggx;
}

float D_Ashikhmin(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float a2 = alpha * alpha;
    float cos2h = NdotH * NdotH;
    float sin2h = 1.0 - cos2h;
    float sin4h = sin2h * sin2h;
    float cot2 = -cos2h / (a2 * sin2h);
    return 1.0 / (MATH_PI * (4.0 * a2 + 1.0) * sin4h) * (4.0 * exp(cot2) + sin4h);
}

float D_Charlie(float sheenRoughness, float NdotH)
{
    sheenRoughness = max(sheenRoughness, 0.000001);
    float invR = 1.0 / sheenRoughness;
    float cos2h = NdotH * NdotH;
    float sin2h = 1.0 - cos2h;
    return (2.0 + invR) * pow(sin2h, invR * 0.5) / (2.0 * MATH_PI);
}

MicrofacetDistributionSample Charlie(vec2 xi, float roughness)
{
    MicrofacetDistributionSample charlie;

    float alpha = roughness * roughness;
    charlie.sinTheta = pow(xi.y, alpha / (2.0*alpha + 1.0));
    charlie.cosTheta = sqrt(1.0 - charlie.sinTheta * charlie.sinTheta);
    charlie.phi = 2.0 * MATH_PI * xi.x;
    charlie.pdf = D_Charlie(alpha, charlie.cosTheta);
    charlie.pdf /= 4.0;

    return charlie;
}

MicrofacetDistributionSample Lambertian(vec2 xi, float roughness)
{
    MicrofacetDistributionSample lambertian;

    lambertian.cosTheta = sqrt(1.0 - xi.y);
    lambertian.sinTheta = sqrt(xi.y);
    lambertian.phi = 2.0 * MATH_PI * xi.x;
    lambertian.pdf = lambertian.cosTheta / MATH_PI;

    return lambertian;
}

vec4 GetImportanceSample(int sampleIndex, vec3 N, float roughness)
{
    MicrofacetDistributionSample importanceSample;

    vec2 xi = Hammersley2d(sampleIndex, _SampleCount);

    if (_Distribution == TypeLambertian)
    {
        importanceSample = Lambertian(xi, roughness);
    }
    else if (_Distribution == TypeGGX)
    {
        importanceSample = GGX(xi, roughness);
    }
    else if (_Distribution == TypeCharlie)
    {
        importanceSample = Charlie(xi, roughness);
    }
    
    vec3 localSpaceDirection = normalize(vec3(
        importanceSample.sinTheta * cos(importanceSample.phi), 
        importanceSample.sinTheta * sin(importanceSample.phi), 
        importanceSample.cosTheta
    ));

    mat3 TBN = GenerateTBN(N);
    vec3 direction = TBN * localSpaceDirection;

    return vec4(direction, importanceSample.pdf);
}

float ComputeLod(float pdf)
{
    float lod = 0.5 * log2( 6.0 * float(_Width) * float(_Width) / (float(_SampleCount) * pdf));
    return lod;
}

vec3 FilterColor(vec3 N)
{
    vec3 color = vec3(0.f);
    float weight = 0.0f;

    for (int i = 0; i < _SampleCount; ++i)
    {
        vec4 importanceSample = GetImportanceSample(i, N, _Roughness);
        vec3 H = vec3(importanceSample.xyz);
        float pdf = importanceSample.w;
        float lod = ComputeLod(pdf) + _LodBias;

        if (_Distribution == TypeLambertian)
        {
            vec3 lambertian = textureLod(_InputCubeMap, H, lod).rgb;
            color += lambertian;
        }
        else if (_Distribution == TypeGGX || _Distribution == TypeCharlie)
        {
            vec3 V = N;
            vec3 L = normalize(reflect(-V, H));
            float NdotL = dot(N, L);

            if (NdotL > 0.0)
            {
                if (_Roughness == 0.0)
                {
                    lod = _LodBias;
                }
                vec3 sampleColor = textureLod(_InputCubeMap, L, lod).rgb;
                color += sampleColor * NdotL;
                weight += NdotL;
            }
        }
    }

    if (weight != 0.0f)
    {
        color /= weight;
    }
    else
    {
        color /= float(_SampleCount);
    }

    return color.rgb ;
}

float V_SmithGGXCorrelated(float NoV, float NoL, float roughness) 
{
    float a2 = pow(roughness, 4.0);
    float GGXV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
    float GGXL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
    return 0.5 / (GGXV + GGXL);
}

float V_Ashikhmin(float NdotL, float NdotV)
{
    return clamp(1.0 / (4.0 * (NdotL + NdotV - NdotL * NdotV)), 0.0, 1.0);
}

vec3 LUT(float NdotV, float roughness)
{
    vec3 V = vec3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);
    vec3 N = vec3(0.0, 0.0, 1.0);

    float A = 0.0;
    float B = 0.0;
    float C = 0.0;

    for (int i = 0; i < _SampleCount; ++i)
    {
        vec4 importanceSample = GetImportanceSample(i, N, roughness);
        vec3 H = importanceSample.xyz;
        vec3 L = normalize(reflect(-V, H));

        float NdotL = Saturate(L.z);
        float NdotH = Saturate(H.z);
        float VdotH = Saturate(dot(V, H));

        if (NdotL > 0.0)
        {
            if (_Distribution == TypeGGX)
            {
                float V_pdf = V_SmithGGXCorrelated(NdotV, NdotL, roughness) * VdotH * NdotL / NdotH;
                float Fc = pow(1.0 - VdotH, 5.0);

                A += (1.0 - Fc) * V_pdf;
                B += Fc * V_pdf;
                C += 0.0;
            }

            if (_Distribution == TypeCharlie)
            {
                float sheenDistribution = D_Charlie(roughness, NdotH);
                float sheenVisibility = V_Ashikhmin(NdotL, NdotV);

                A += 0.0;
                B += 0.0;
                C += sheenVisibility * sheenDistribution * NdotL * VdotH;
            }
        }
    }

    return vec3(4.0 * A, 4.0 * B, 4.0 * 2.0 * MATH_PI * C) / float(_SampleCount);
}

void main()
{
    vec3 color = vec3(0);

    if (_GeneratingLUT == 0)
    {
        vec2 newUV = texCoord * 2.0 - 1.0;
        vec3 scan = UVToXYZ(_CurrentFace, newUV);
        
        vec3 direction = normalize(scan);
        direction.y = -direction.y;

        color = FilterColor(direction);
    }
    else
    {
        color = LUT(texCoord.x, texCoord.y);
    }
    
    fragmentColor = vec4(color,1.0);
}