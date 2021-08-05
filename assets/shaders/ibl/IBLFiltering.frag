#version 330 core

precision mediump float;

#define MATH_PI 3.1415926535897932384626433832795

uniform samplerCube uCubeMap;

// enum
const int cLambertian = 0;
const int cGGX = 1;
const int cCharlie = 2;

uniform float   u_roughness;
uniform int     u_sampleCount;
uniform int     u_width;
uniform float   u_lodBias;
uniform int     u_distribution;
uniform int     u_currentFace;

in vec2  texCoord;
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
        return vec3(uv.x, -1.0f, uv.y);
    }
	else if (face == 3)
    {
        return vec3(uv.x, 1.0f, -uv.y);
    }
	else if (face == 4)
	{
        return vec3(uv.x, uv.y, 1.0f);
    }
	else
    {
		return vec3(-uv.x, uv.y, -1.0f);
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

float BitfieldReverse(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits);
}

float Hammersley(uint i)
{
    return BitfieldReverse(i) * 2.3283064365386963e-10;
}

vec3 GetImportanceSampleDirection(vec3 normal, float sinTheta, float cosTheta, float phi)
{
	vec3 H = normalize(vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta));

    vec3 bitangent = vec3(0.0, 1.0, 0.0);

	float NdotX = dot(normal, vec3(1.0, 0.0, 0.0));
	float NdotY = dot(normal, vec3(0.0, 1.0, 0.0));
	float NdotZ = dot(normal, vec3(0.0, 0.0, 1.0));

	if (abs(NdotY) > abs(NdotX) && abs(NdotY) > abs(NdotZ))
	{
		if (NdotY > 0.0)
		{
			bitangent = vec3(0.0, 0.0, 1.0);
		}
		else
		{
			bitangent = vec3(0.0, 0.0, -1.0);
		}
	}

    vec3 tangent = cross(bitangent, normal);
    bitangent = cross(normal, tangent);

	return normalize(tangent * H.x + bitangent * H.y + normal * H.z);
}

float V_Ashikhmin(float NdotL, float NdotV)
{
    return clamp(1.0 / (4.0 * (NdotL + NdotV - NdotL * NdotV)), 0.0, 1.0);
}

float D_GGX(float NdotH, float roughness)
{
    float alpha   = roughness * roughness;
    float alpha2  = alpha * alpha;
    float divisor = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
    return alpha2 / (MATH_PI * divisor * divisor);
}

float D_Ashikhmin(float NdotH, float roughness)
{
	float alpha = roughness * roughness;
	float a2    = alpha * alpha;
	float cos2h = NdotH * NdotH;
	float sin2h = 1.0 - cos2h;
	float sin4h = sin2h * sin2h;
	float cot2  = -cos2h / (a2 * sin2h);
	return 1.0 / (MATH_PI * (4.0 * a2 + 1.0) * sin4h) * (4.0 * exp(cot2) + sin4h);
}

float D_Charlie(float sheenRoughness, float NdotH)
{
    sheenRoughness = max(sheenRoughness, 0.000001); //clamp (0,1]
    float alphaG = sheenRoughness * sheenRoughness;
    float invR  = 1.0 / alphaG;
    float cos2h = NdotH * NdotH;
    float sin2h = 1.0 - cos2h;
    return (2.0 + invR) * pow(sin2h, invR * 0.5) / (2.0 * MATH_PI);
}

vec3 GetSampleVector(int sampleIndex, vec3 N, float roughness)
{
	float X = float(sampleIndex) / float(u_sampleCount);
	float Y = Hammersley(uint(sampleIndex));

	float phi = 2.0 * MATH_PI * X;
    float cosTheta = 0.f;
	float sinTheta = 0.f;

	if (u_distribution == cLambertian)
	{
		cosTheta = 1.0 - Y;
		sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	}
	else if (u_distribution == cGGX)
	{
		float alpha = roughness * roughness;
		cosTheta = sqrt((1.0 - Y) / (1.0 + (alpha * alpha - 1.0) * Y));
		sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	}
	else if (u_distribution == cCharlie)
	{
		float alpha = roughness * roughness;
		sinTheta = pow(Y, alpha / (2.0 * alpha + 1.0));
		cosTheta = sqrt(1.0 - sinTheta * sinTheta);
	}

	return GetImportanceSampleDirection(N, sinTheta, cosTheta, phi);
}

float PDF(vec3 V, vec3 H, vec3 N, vec3 L, float roughness)
{
	if (u_distribution == cLambertian)
	{
		float NdotL = dot(N, L);
		return max(NdotL * (1.0 / MATH_PI), 0.0);
	}
	else if (u_distribution == cGGX)
	{
		float VdotH = dot(V, H);
		float NdotH = dot(N, H);
		float D = D_GGX(NdotH, roughness);
		return max(D * NdotH / (4.0 * VdotH), 0.0);
	}
	else if (u_distribution == cCharlie)
	{
		float VdotH = dot(V, H);
		float NdotH = dot(N, H);
		float D = D_Charlie(roughness, NdotH);
		return max(D * NdotH / abs(4.0 * VdotH), 0.0);
	}
	return 0.f;
}

vec3 FilterColor(vec3 N)
{
	vec4 color = vec4(0.f);
	float solidAngleTexel = 4.0 * MATH_PI / (6.0 * float(u_width) * float(u_width));

	for (int i = 0; i < u_sampleCount; ++i)
	{
		vec3 H = GetSampleVector(i, N, u_roughness);
		vec3 V = N;
		vec3 L = normalize(reflect(-V, H));

		float NdotL = dot(N, L);

		if (NdotL > 0.0)
		{
			float lod = 0.0;

			if (u_roughness > 0.0 ||u_distribution == cLambertian)
			{
				float pdf = PDF(V, H, N, L, u_roughness);
				float solidAngleSample = 1.0 / (float(u_sampleCount) * pdf);
				lod  = 0.5 * log2(solidAngleSample / solidAngleTexel);
				lod += u_lodBias;
			}

			if (u_distribution == cLambertian)
			{
				color += vec4(textureLod(uCubeMap, H, lod).rgb, 1.0);
			}
			else
			{
				color += vec4(textureLod(uCubeMap, L, lod).rgb * NdotL, NdotL);
			}
		}
	}

	if (color.w == 0.f)
	{
		return color.rgb;
	}

	return color.rgb / color.w;
}

float V_SmithGGXCorrelated(float NoV, float NoL, float roughness)
{
	float a2 = pow(roughness, 4.0);
	float GGXV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
	float GGXL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
	return 0.5 / (GGXV + GGXL);
}

vec3 LUT(float NdotV, float roughness)
{
	vec3 V  = vec3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);
	vec3 N  = vec3(0.0, 0.0, 1.0);
	float A = 0.0;
	float B = 0.0;
	float C = 0.0;

	for (int i = 0; i < u_sampleCount; ++i)
	{
		vec3 H = GetSampleVector(i, N, roughness);
		vec3 L = normalize(reflect(-V, H));

		float NdotL = Saturate(L.z);
		float NdotH = Saturate(H.z);
		float VdotH = Saturate(dot(V, H));

		if (NdotL > 0.0)
		{
			if (u_distribution == cGGX)
			{
				float V_pdf = V_SmithGGXCorrelated(NdotV, NdotL, roughness) * VdotH * NdotL / NdotH;
				float Fc = pow(1.0 - VdotH, 5.0);
				A += (1.0 - Fc) * V_pdf;
				B += Fc * V_pdf;
				C += 0.0;
			}

			if (u_distribution == cCharlie)
			{
				float sheenDistribution = D_Charlie(roughness, NdotH);
				float sheenVisibility = V_Ashikhmin(NdotL, NdotV);

				A += 0.0;
				B += 0.0;
				C += sheenVisibility * sheenDistribution * NdotL * VdotH;
			}
		}
	}

	return vec3(4.0 * A, 4.0 * B, 4.0 * 2.0 * MATH_PI * C) / float(u_sampleCount);
}

void main()
{
	vec2 newUV = texCoord * 2.0 - 1.0;
	vec3 scan  = UVToXYZ(u_currentFace, newUV);

	vec3 direction = normalize(scan);
	direction.y = -direction.y;

	vec3 color = FilterColor(direction);

	fragmentColor = vec4(color, 1.0);
}
