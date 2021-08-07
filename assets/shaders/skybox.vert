#version 330 core

layout (location = 0) in vec3 inPosition;

uniform mat4 _MVP;

out vec3 varyUVW;

void main()
{
    gl_Position = _MVP * vec4(inPosition, 1.0);
	gl_Position.z = gl_Position.w;

    varyUVW     = normalize(inPosition.xyz);
}