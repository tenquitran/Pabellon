#version 440 core
#pragma debug(on)
#pragma optimize(off)

in vec2 texCoord1;
out vec4 fragColor;

uniform sampler2D uSampler;

void main()
{
	fragColor = vec4(texture(uSampler, texCoord1));

	//fragColor = vec4(0.0, 1.0, 1.0, 1.0);
}
