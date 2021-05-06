#include "VAO.h"
#include "Graphic.h"
#include "Shader.h"
#include "Buffer.h"
VAO::VAO(Shader* shaderData) : shader(nullptr), buffer(nullptr)
{
	shader = shaderData;
	glGenVertexArrays(1, &vaoId);
}


void VAO::Init(std::vector<Vertex>& datas, bool onlyPos)
{
	if(shader != nullptr)
	{
		shader->Use();
	}
	glBindVertexArray(vaoId);
	buffer = new Buffer(GL_ARRAY_BUFFER, (unsigned)(datas.size() * sizeof(Vertex)), GL_STATIC_DRAW, (void*)datas.data());
	buffer->Bind();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	if (!onlyPos)
	{
		if (datas[0].normal != Vector3())
		{
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
			glEnableVertexAttribArray(1);
		}
		if (datas[0].texCoord != Vector2())
		{
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));
			glEnableVertexAttribArray(2);
		}
	}
}

void VAO::Init(float* data, int size, int indexNum, const std::vector<int>& sizePerIndex)
{
	if (shader != nullptr)
	{
		shader->Use();
	}
	glBindVertexArray(vaoId);

	buffer = new Buffer(GL_ARRAY_BUFFER, size * sizeof(float), GL_STATIC_DRAW, (void*)data);
	buffer->Bind();

	const size_t sizeVecSize = sizePerIndex.size();
	int stride = 0;
	for (size_t i = 0; i < sizeVecSize; ++i)
	{
		stride += sizePerIndex[i];
	}
	size_t offset = 0;
	for (int i = 0; i < indexNum; ++i)
	{
		const unsigned sizeOfIndex = sizePerIndex[i];
		glVertexAttribPointer(i, sizeOfIndex, GL_FLOAT, GL_FALSE, stride * sizeof(float), (GLvoid*)offset);
		glEnableVertexAttribArray(i);
		offset += sizePerIndex[i] * sizeof(float);
	}
}

void VAO::Bind()
{
	if (shader != nullptr)
	{
		shader->Use();
	}
	glBindVertexArray(vaoId);
}

unsigned VAO::GetId()
{
	return vaoId;
}

VAO::~VAO()
{
	glDeleteVertexArrays(1, &vaoId);
	if (buffer != nullptr)
	{
		delete buffer;
	}
}
