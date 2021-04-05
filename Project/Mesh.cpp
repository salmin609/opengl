/*************************************************************
 *	Author				: Kim sang min.
 *	Assignment number	: Project assignment
 *	Course				: CS250.
 *  Term				: Spring 2020.
 *************************************************************/

#include <stdexcept>
#include <fstream>
#include <sstream>
#include <utility>
#include "Mesh.h"

#include <iostream>
#include <random>

#include "RandomNumGenerator.h"
#include "vs2017/SimpleMeshes.h"

unsigned Mesh::Get_VAO_Id()
{
	return vao_id;
}

unsigned Mesh::Get_Shader_Id()
{
	return shader->GetShaderId();
}

unsigned Mesh::Get_Texture_Id() const
{
	return texture_id;
}

unsigned Mesh::Get_Seconde_Texture_Id() const
{
	return specular_texture_id;
}

std::vector<unsigned>& Mesh::GetTextureId()
{
	return textureId;
}

void Mesh::Initialize(const char* vertexPath, const char* fragmentPath)
{
	shader = new Shader(vertexPath, fragmentPath);

	Init_VAO();

	Init_VBO(vertexDatas.data(), &vbo_id, vertexDatas.size() * sizeof(Vertex),
		sizeof(Vertex), (GLvoid*)offsetof(Vertex, position), 0, 3);

	if (!elements.empty())
	{
		glGenBuffers(1, &elementId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), elements.data(), GL_STATIC_DRAW);
	}
}

void Mesh::Initialize_Object_Mesh(std::string vertex_path, std::string frag_path)
{
	Initialize(vertex_path.c_str(), frag_path.c_str());

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
}

void Mesh::InitializeTexturedObj(std::string sprite_path, std::string vertex_path, std::string frag_path)
{
	Initialize_Object_Mesh(std::move(vertex_path), std::move(frag_path));

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);
	
	Initialize_Texture(std::move(sprite_path), 0, 0);
}

void Mesh::InitializeInstanceObj(std::string spritePath, std::string vertexPath, std::string fragPath)
{
	InitializeTexturedObj(std::move(spritePath), std::move(vertexPath), std::move(fragPath));
	instancingNum = 250;
	const int offsetDivisor = 10;
	bool minusTrigger = false;

	for (size_t i = 0; i < instancingNum; ++i)
	{
		const int randomOffsetx = rand() % offsetDivisor;
		const int randomOffsety = rand() % (offsetDivisor / 2);
		const int randomOffsetz = rand() % offsetDivisor;

		if (!minusTrigger)
		{
			offsetVec.push_back(Vector3{
				static_cast<float>(randomOffsetx),
				static_cast<float>(randomOffsety),
				static_cast<float>(randomOffsetz) });
		}
		else
		{
			offsetVec.push_back(Vector3{
				static_cast<float>(-randomOffsetx),
				static_cast<float>(-randomOffsety),
				static_cast<float>(-randomOffsetz) });
		}
		minusTrigger = !minusTrigger;
	}

	Init_VBO(offsetVec.data(), &instancingId, sizeof(Vector3) * offsetVec.size(),
		0, 0, 3, 3);
	glVertexAttribDivisor(3, 1);
}

void Mesh::InitializeColoredParticle(std::string vertexPath, std::string fragmentPath)
{
	int simpleCubeSize;
	float* simpleCubeVertices = SimpleMesh::SimpleCube(simpleCubeSize);

	shader = new Shader(vertexPath.c_str(), fragmentPath.c_str());

	Init_VAO();

	Init_VBO(simpleCubeVertices, &vbo_id, simpleCubeSize * sizeof(float),
		0, 0, 0, 3);

	Init_VBO(particles.data(), &color_id, particles.size() * sizeof(ParticleInstance),
		sizeof(ParticleInstance), (GLvoid*)offsetof(ParticleInstance, color), 1, 3);

	Init_VBO(NULL, &matrixId, particles.size() * sizeof(Vector3),
		0, 0, 2, 3, GL_STREAM_DRAW);

	Init_VBO(NULL, &rotateId, particles.size() * sizeof(Vector3),
		0, 0, 3, 3, GL_STREAM_DRAW);

	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);

	delete[] simpleCubeVertices;
}


void Mesh::Initialize_Texture(std::string sprite_path, int width, int height, unsigned textureNum)
{
	unsigned* newId = new unsigned(textureNum);
	glGenTextures(textureNum, newId);

	for(unsigned i = 0 ; i < textureNum; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, newId[i]);
		glActiveTexture(GL_TEXTURE_2D);
		textureId.push_back(newId[i]);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Image temp_image;
	int w, h;
	unsigned char* data = temp_image.Load_Image(std::move(sprite_path), w, h, true);
	if(data != nullptr)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
		
}

void Mesh::Clear_Datas()
{
	//vertex_datas.clear();
	//normal_datas.clear();
	//texture_coords.clear();
}

bool Mesh::Get_Is_Textured() const
{
	return is_textured;
}


/**
 * \brief
 * Check if texture is exist on the mesh.
 * If so, activate the texture depends on the texture slot's size.
 */
void Mesh::SetTexture()
{
	const size_t textureSlotSize = textureId.size();
	if (textureSlotSize > 0)
	{
		for (unsigned int i = 0; i < textureSlotSize; ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textureId[i]);
		}
	}
}

void Mesh::PushTextureId(unsigned id)
{
	textureId.push_back(id);
}

void Mesh::ClearTextureIds()
{
	textureId.clear();
}

bool Mesh::isTextureSlotEmpty()
{
	return textureId.empty();
}

bool Mesh::IsElemented()
{
	return !elements.empty();
}

unsigned Mesh::GetElementId()
{
	return elementId;
}

bool Mesh::IsQuadObj()
{
	return isQuad;
}

Shader* Mesh::GetShader()
{
	return shader;
}

void Mesh::Init_VAO()
{
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);
}

void Mesh::Init_VBO(void* data, unsigned* slot, size_t arr_size, int stride, void* offset, int index, int vec_size, GLenum drawingType
	, bool isNormalize)
{
	if (arr_size > 0)
	{
		glGenBuffers(1, slot);
		glBindBuffer(GL_ARRAY_BUFFER, *slot);
		glBufferData(GL_ARRAY_BUFFER, arr_size, data, drawingType);
		if (!isNormalize)
		{
			glVertexAttribPointer(index, vec_size, GL_FLOAT, GL_FALSE, stride, offset);
		}
		else
		{
			glVertexAttribPointer(index, vec_size, GL_FLOAT, GL_TRUE, stride, offset);
		}
		glEnableVertexAttribArray(index);
	}
	else
	{
		//throw std::runtime_error("Error! : Initialize VBO but the size of array is 0");
	}

}

void Mesh::Unbind()
{
	glBindVertexArray(0);
	//glUseProgram(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}


bool Mesh::IsInstancing()
{
	return isInstancing;
}

int Mesh::InstancingNum() const
{
	return instancingNum;
}

void Mesh::MoveParticle(float dt)
{
	const float speed = 2.f;
	const int randomIndex = RandomNumber::RandomInt(10);
	instancingNum += randomIndex;

	if(instancingNum > particleNum)
	{
		instancingNum = particleNum;
	}
	std::vector<Vector3> dirVec;
	std::vector<Vector3> rotateVec;
	
	for(int i = 0; i < instancingNum; ++i)
	{
		ParticleInstance& particle = particles[i];

		particle.life -= dt;
		if (particle.life < 0.f)
		{
			instancingNum--;
			particle.life = 2.f;
			particle.dir.x = RandomNumber::RandomFloat(0.f, 10.f);
			particle.dir.y = RandomNumber::RandomFloat(0.f, 10.f);
			particle.dir.z = RandomNumber::RandomFloat(0.f, 10.f);
		}
		else
		{
			particle.dir.x += (particle.dir.x * speed * dt);
			particle.dir.y += (particle.dir.y * speed * dt);
			particle.dir.z += (particle.dir.z * speed * dt);
		}
		dirVec.push_back(particle.dir);
		rotateVec.push_back(particle.rotate);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, matrixId);
	
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		dirVec.size() * sizeof(Vector3),
		dirVec.data());

	glBindBuffer(GL_ARRAY_BUFFER, rotateId);

	glBufferSubData(GL_ARRAY_BUFFER, 0,
		rotateVec.size() * sizeof(Vector3),
		rotateVec.data());
}