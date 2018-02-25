#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 texCoord;

uniform mat4 mvp;

out vec2 texCoord1;

void main()
{
	gl_Position = mvp * vPosition;

	texCoord1 = texCoord;
}
