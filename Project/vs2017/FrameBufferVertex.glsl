#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 textureCoord;

out vec2 texCoord;

void main()
{
	texCoord = textureCoord;
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}
