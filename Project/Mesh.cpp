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
#include "Graphic.h"
#include "vs2017/SimpleMeshes.h"
#include "VAO.h"
#include "RandomNumGenerator.h"
#include "Texture.h"
unsigned Mesh::Get_VAO_Id()
{
	if(vao != nullptr)
	{
		return vao->GetId();
	}
	return vao_id;
}

unsigned Mesh::Get_Shader_Id()
{
	return shader->GetShaderId();
}


void Mesh::Initialize(const char* vertexPath, const char* fragmentPath)
{
	shader = new Shader(vertexPath, fragmentPath);
	vao = new VAO(shader);
	vao->Init(vertexDatas);

	if (!elements.empty())
	{
		glGenBuffers(1, &elementId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), elements.data(), GL_STATIC_DRAW);
	}
}

void Mesh::InitializeTexturedObj(std::string spritePath, std::string vertex, std::string fragment)
{
	Initialize_Texture(spritePath);
	Initialize(vertex.c_str(), fragment.c_str());
}


void Mesh::InitializeInstanceObj(std::string spritePath, std::string vertexPath, std::string fragPath)
{
	Initialize_Texture(std::move(spritePath));
	Initialize(vertexPath.c_str(), fragPath.c_str());
	instancingNum = 500;
	//const int offsetDivisor = 10;
	//bool minusTrigger = false;

	for (size_t i = 0; i < instancingNum; ++i)
	{
		Vector3 randomVec = RandomNumber::instance->RandomVector3(-10.f, 10.f);
		offsetVec.push_back(randomVec);
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

	Init_VBO(NULL, &color_id, particles.size() * sizeof(Vector3),
		0, 0, 1, 3, GL_STREAM_DRAW);

	Init_VBO(NULL, &matrixId, particles.size() * sizeof(Vector3),
		0, 0, 2, 3, GL_STREAM_DRAW);

	Init_VBO(NULL, &rotateId, particles.size() * sizeof(Vector3),
		0, 0, 3, 3, GL_STREAM_DRAW);

	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);

	delete[] simpleCubeVertices;
}


void Mesh::Initialize_Texture(std::string sprite_path)
{
	textures.push_back(new Texture(sprite_path));
}


/**
 * \brief
 * Check if texture is exist on the mesh.
 * If so, activate the texture depends on the texture slot's size.
 */
void Mesh::SetTexture()
{
	const size_t textureSlotSize = textures.size();
	if (textureSlotSize > 0)
	{
		for (unsigned int i = 0; i < textureSlotSize; ++i)
		{
			textures[i]->Bind(i);
		}
	}
}

bool Mesh::IsElemented()
{
	return !elements.empty();
}

unsigned Mesh::GetElementId()
{
	return elementId;
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
		throw std::runtime_error("Error! : Initialize VBO but the size of array is 0");
	}

}

void Mesh::Unbind()
{
	glBindVertexArray(0);

	const size_t texturesSize = textures.size();

	for(size_t i = 0 ; i < texturesSize; ++i)
	{
		textures[i]->Unbind();
	}
}

Mesh::~Mesh(void)
{
	glDeleteVertexArrays(1, &vao_id);
	glDeleteBuffers(1, &vbo_id);
	delete shader;

	const size_t texturesSize = textures.size();
	for(size_t i = 0 ; i < texturesSize; ++i)
	{
		delete textures[i];
	}
}



bool Mesh::IsInstancing()
{
	return isInstancing;
}

int Mesh::InstancingNum() const
{
	return instancingNum;
}

std::vector<Vertex> Mesh::GetVertexDatas()
{
	return vertexDatas;
}

void Mesh::Bind()
{
	if(vao != nullptr)
	{
		vao->Bind();
	}
}
