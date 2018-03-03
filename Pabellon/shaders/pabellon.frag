#version 440 core
#pragma debug(on)
#pragma optimize(off)

in vec2 texCoord1;
//in vec3 color;

out vec4 fragColor;

uniform sampler2D uSampler;

uniform bool HasTextureCoords;

uniform vec3 MaterialAmbient;
uniform vec3 MaterialDiffuse;
uniform vec3 MaterialSpecular;
uniform float MaterialShininess;

// Ambient, diffuse and specular light intensity.
// TODO: hard-coded.
uniform vec3 LightAmbient  = vec3(1.0, 1.0, 1.0);
uniform vec3 LightDiffuse  = vec3(1.0, 1.0, 1.0);
uniform vec3 LightSpecular = vec3(1.0, 1.0, 1.0);

in VS_OUT
{
	vec3 normal;
	vec3 light;
	vec3 view;
} fs_in;

void main()
{
	//fragColor = vec4(texture(uSampler, texCoord1));

	//fragColor = vec4(texture(uSampler, texCoord1)) + Color;

	vec3 r = reflect(-fs_in.light, fs_in.normal);

	// Calculate ambient, diffuse and specular contributions.

	vec3 ambient  = LightAmbient * MaterialAmbient;

	float dotN = max(dot(fs_in.normal, fs_in.light), 0.0);

	vec3 diffuse  = dotN * LightDiffuse * MaterialDiffuse;

	vec3 specular = vec3(0.0);
	if (dotN > 0.0)
	{
		specular = pow(max(dot(r, fs_in.view), 0.0), MaterialShininess) * LightSpecular * MaterialSpecular;
	}

	//vec3 color = ambient + diffuse + specular;

	if (true == HasTextureCoords)
	{
		vec4 texColor = vec4(texture(uSampler, texCoord1));

		fragColor = (vec4(ambient + diffuse, 1.0) * texColor) + vec4(specular, 1.0);

		//fragColor = vec4(texture(uSampler, texCoord1));
	}
	else
	{
		vec3 color = ambient + diffuse + specular;

		if (color.r > 1.0 || color.g > 1.0 || color.b > 1.0)
		{
			//fragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
			fragColor = vec4(1.0, 0.0, 0.0, 1.0);
		}
		else
		{
			fragColor = vec4(color, 1.0);
		}
	}

	//fragColor = vec4(0.0, 1.0, 1.0, 1.0);
}
