#include "Water.h"

Water::Water()
{
	Water::SetVertices();
}

void Water::SetVertices()
{
	float v[] = { -1.f, -1.f, -1.f,
						1.f, 1.f, -1.f,
						-1.f, 1.f, -1.f,
						-1.f, -1.f, -1.f,
						1.f, -1.f, -1.f,
						1.f, 1.f, 1.f };

	shader = new Shader(shaderWaterVertex.c_str(), shaderWaterFragment.c_str());
	

	Init_VAO();
	Init_VBO(v, &vbo_id, sizeof(v), 0, 0, 0, 3);
	Initialize_Texture(std::string(), 200, 200, 1);
	//Initialize_Texture(std::string(), 200, 200);
	shader->SendUniformInt("reflect", 0);
	shader->SendUniformInt("refract", 1);
	//ClearTextureIds();
}
