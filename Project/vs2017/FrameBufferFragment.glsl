#version 330 core
out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D screenTextures;

void main()
{
	vec3 col = texture(screenTextures, texCoord).rgb;

	fragColor = vec4(col, 1.0);
}