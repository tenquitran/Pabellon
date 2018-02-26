#version 440 core
#pragma debug(on)
#pragma optimize(off)

in vec2 texCoord1;
out vec4 fragColor;

uniform sampler2D uSampler;

in VS_OUT
{
	vec3 normal;
	vec3 light;
	vec3 view;
} fs_in;

void main()
{
	vec3 r = reflect(-fs_in.light, fs_in.normal);

	fragColor = vec4(texture(uSampler, texCoord1));

	//fragColor = vec4(0.0, 1.0, 1.0, 1.0);
}
