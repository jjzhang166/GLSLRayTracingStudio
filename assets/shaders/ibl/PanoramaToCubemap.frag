#version 330 core

precision highp float;

#define MATH_PI 3.1415926535897932384626433832795
#define MATH_INV_PI (1.0 / MATH_PI)

in vec2 texCoord;
out vec4 fragmentColor;

uniform int _CurrentFace;
uniform sampler2D _InputTexture;

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

vec3 PanoramaToCubeMap(int face, vec2 texCoord)
{
	vec2 texCoordNew = texCoord*2.0-1.0;
	vec3 scan = UVToXYZ(face, texCoordNew);
	vec3 direction = normalize(scan);
	vec2 src = DirToUV(direction);

	return texture(_InputTexture, src).rgb;
}

void main(void)
{
	fragmentColor.rgb = PanoramaToCubeMap(_CurrentFace, texCoord);
	fragmentColor.a = 1.0;
}
