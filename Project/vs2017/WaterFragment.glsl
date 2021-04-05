#version 330 core
out vec4 fragColor;

in vec4 clipSpace;

uniform sampler2D reflect;
uniform sampler2D refract;

void main(void)
{
	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
	vec2 refractTexCoords = vec2(ndc.x, ndc.y);
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
	
	vec4 reflectColor = texture(reflect, reflectTexCoords);
	vec4 refractColor = texture(refract, refractTexCoords);

	fragColor = mix(reflectColor, refractColor, 0.5);
}