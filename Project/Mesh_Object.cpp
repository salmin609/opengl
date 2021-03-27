#include "Mesh_Object.h"

unsigned Mesh_Object::GetVAOId()
{
	return vao_id;
}

unsigned Mesh_Object::GetShaderId()
{
	return shader->GetShaderId();
}

void Mesh_Object::InitializeElement()
{
	const int faceSize = FaceCount();

	for(int i = 0 ; i < faceSize; ++i)
	{
		Face face = GetFace(i);
		const int index1 = face.index1;
		const int index2 = face.index2;
		const int index3 = face.index3;
		
		elements.push_back(index1);
		elements.push_back(index2);
		elements.push_back(index3);
	}
}

void Mesh_Object::SetNormal()
{
	const int faceSize = FaceCount();

	for (int i = 0; i < faceSize; ++i)
	{
		Face face = GetFace(i);
		const int index1 = face.index1;
		const int index2 = face.index2;
		const int index3 = face.index3;

		Point P = vertices[index1];
		Point Q = vertices[index2];
		Point R = vertices[index3];

		const Vector Q_minus_P = Q - P;
		const Vector R_minus_P = R - P;

		Vector orientation_vec(cross(Q_minus_P, R_minus_P));

		orientation_vec = normalize(orientation_vec);

		normal[index1] += orientation_vec;
		normal[index2] += orientation_vec;
		normal[index3] += orientation_vec;
	}
}

void Mesh_Object::InitializeVerticesDatas()
{
	const size_t verticesSize = vertices.size();

	for (size_t i = 0; i < verticesSize; ++i)
	{
		Vertex data{ Vector3{ vertices[i].x, vertices[i].y, vertices[i].z } ,
			Vector3{normal[i].x, normal[i].y, normal[i].z}};
		vertexDatas.push_back(data);
	}
}

void Mesh_Object::InitializeTextureObj(std::string sprite_path)
{
	SetNormal();
	InitializeVerticesDatas();
	InitializeElement();
	InitializeTexturedObj(std::move(sprite_path));
}
