#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

//uniform mat4 View;
uniform mat4 ModelView;
//uniform mat4 Projection;
uniform mat4 MVP;
uniform mat3 Normal;

out VS_OUT
{
	vec3 normal;
	vec3 light;
	vec3 view;
} vs_out;

// Light position.
// TODO: hard-coded.
//uniform vec3 LightPos = vec3(0.0, 100.0, 0.0);
uniform vec3 LightPos = vec3(100.0, 100.0, 100.0);

out vec2 texCoord1;
//out vec3 color;

void main()
{
	// Calculate view-space coordinate.
	vec4 viewPos = ModelView * position;

	// Calculate normal in view-space.
	vs_out.normal = normalize(Normal * normal);
	//vs_out.normal = normalize(mat3(ModelView) * normal);

	// Calculate view-space light vector.
	vs_out.light = normalize(LightPos - viewPos.xyz);

	// Calculate view vector.
	vs_out.view = normalize(-viewPos.xyz);

	gl_Position = MVP * viewPos;

	texCoord1 = texCoord;
}
