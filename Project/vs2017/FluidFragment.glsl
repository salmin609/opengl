#version 330 core

out vec4 outColor;
in vec3 color;

uniform vec3 lightDir;

void main()
{
	vec3 N;
	N.xy = gl_PointCoord * 2.0 - vec2(1.0);
	float mag = dot(N.xy, N.xy);

	if(mag > 1.0)
	{
		discard;
	}
	N.z = sqrt(1.0 - mag);
	float diffuse = dot(N, lightDir);

	if(diffuse < 0.0f)
	{
		diffuse = -diffuse;
	}
	outColor = vec4(color * diffuse, 1.0);
}