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
#include "Image.h"
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

	
	Init_VBO(vertexDatas.data(), &vbo_id, vertexDatas.size() * sizeof(Vertex),
		sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal), 1, 3);
}

void Mesh::InitializeTexturedObj(std::string sprite_path, std::string vertex_path, std::string frag_path)
{
	Initialize_Object_Mesh(std::move(vertex_path), std::move(frag_path));

	Init_VBO(vertexDatas.data(), &texture_id, vertexDatas.size() * sizeof(Vertex), 
		sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord), 2, 2);

	Initialize_Texture(std::move(sprite_path));
}

void Mesh::InitializeInstanceObj(std::string spritePath, std::string vertexPath, std::string fragPath)
{
	InitializeTexturedObj(std::move(spritePath), std::move(vertexPath), std::move(fragPath));
	instancingNum = 250;
	const int offsetDivisor = 10;
	bool minusTrigger = false;
	
	for(size_t i = 0 ; i < instancingNum; ++i)
	{
		const int randomOffsetx = rand() % offsetDivisor;
		const int randomOffsety = rand() % (offsetDivisor / 2);
		const int randomOffsetz = rand() % offsetDivisor;

		if(!minusTrigger)
		{
			offsetVec.push_back(Vector3{
				static_cast<float>(randomOffsetx),
				static_cast<float>(randomOffsety),
				static_cast<float>(randomOffsetz)});
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
	instancingNum = 100;
	particleNum = 100;
	int offset = 30;
	
	std::vector<Image::Color_Ub> particleColor;

	//color
	for(size_t i = 0; i < particleNum; ++i)
	{
		const int randomx = rand() % 255;
		const int randomy = rand() % 255;
		const int randomz = rand() % 255;

		particleColor.push_back(Image::Color_Ub{ 
			static_cast<unsigned char>(randomx), 
			static_cast<unsigned char>(randomy), 
			static_cast<unsigned char>(randomz),
			1 });
	}
	

	//trans
	for(size_t i = 0 ; i < particleNum; ++i)
	{
		const int randomx = rand() % offset;
		const int randomy = rand() % offset;
		const int randomz = rand() % offset;

		particleTranslation.push_back(Vector{
			static_cast<float>(randomx),
			static_cast<float>(randomy),
			static_cast<float>(randomz)
		});
	}

	Init_VBO(simpleCubeVertices, &vbo_id, simpleCubeSize * sizeof(float),
		0, 0, 0, 3);

	Init_VBO(particleColor.data(), &color_id, 
		particleColor.size() * sizeof(Image::Color_Ub),
		0, 0, 1, 3, GL_STATIC_DRAW, true);

	Init_VBO(NULL, &matrixId,
		particleTranslation.size() * sizeof(Vector),
		0, 0, 2, 3, GL_STREAM_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0,
		particleTranslation.size() * sizeof(Vector),
		particleTranslation.data());

	/*glGenBuffers(1, &matrixId);
	glBindBuffer(GL_ARRAY_BUFFER, matrixId);
	glBufferData(GL_ARRAY_BUFFER, particleTranslation.size() * sizeof(Vector),
		particleTranslation.data(), GL_STREAM_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_FALSE,
		0, 0);*/
	
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);

	delete[] simpleCubeVertices;
}


void Mesh::Initialize_Texture(std::string sprite_path)
{
	unsigned newId;
	glGenTextures(1, &newId);
	glBindTexture(GL_TEXTURE_2D, newId);
	textureId.push_back(newId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Image temp_image;
	int w, h;
	unsigned char* data = temp_image.Load_Image(std::move(sprite_path), w, h, true);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
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
		if(!isNormalize)
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

void Mesh::MoveParticle()
{
	const size_t vecSize = particleTranslation.size();

	for(size_t i = 0 ; i < vecSize; ++i)
	{
		Vector& particleVec = particleTranslation[i];

		particleVec.x += (particleVec.x * 0.001f);
		particleVec.y += (particleVec.y * 0.001f);
		particleVec.z += (particleVec.z * 0.001f);
	}

	glBindBuffer(GL_ARRAY_BUFFER, matrixId);
	glBufferData(GL_ARRAY_BUFFER,
		particleTranslation.size() * sizeof(Vector),
		NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 
		particleTranslation.size() * sizeof(Vector),
		particleTranslation.data());
}

