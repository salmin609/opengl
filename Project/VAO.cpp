#include "VAO.h"
#include "Graphic.h"
#include "Shader.h"

VAO::VAO(Shader* shaderData)
{
	shader = shaderData;
	glGenVertexArrays(1, &vaoId);
}


void VAO::Init(const std::vector<Vertex>& datas)
{
	glBindVertexArray(vaoId);
	
	glGenBuffers(1, &vboSlotId);
	glBindBuffer(GL_ARRAY_BUFFER, vboSlotId);
	glBufferData(GL_ARRAY_BUFFER, datas.size() * sizeof(Vertex), datas.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);
}

unsigned VAO::GetId()
{
	return vaoId;
}

VAO::~VAO()
{
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vboSlotId);
}
