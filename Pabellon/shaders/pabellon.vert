#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 View;
uniform mat4 ModelView;
uniform mat4 Projection;

//uniform mat4 mvp;

out VS_OUT
{
	vec3 normal;
	vec3 light;
	vec3 view;
} vs_out;

// TODO: hard-coded light position.
uniform vec3 LightPos = vec3(100.0, 100.0, 100.0);

out vec2 texCoord1;

void main()
{
	// Calculate view-space coordinate.
	vec4 viewPos = ModelView * position;

	// Calculate normal in view-space.
	vs_out.normal = normalize(mat3(ModelView) * normal);

	// Calculate light vector.
	vs_out.light = normalize(LightPos - viewPos.xyz);

	// Calculate view vector.
	vs_out.view = normalize(-viewPos.xyz);

	gl_Position = Projection * viewPos;

	//gl_Position = mvp * position;

	texCoord1 = texCoord;
}
